#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import sys
import os
import argparse
import glob
#import cStringIO

from sys import version_info

if version_info.major == 3:
    import importlib
    importlib.reload(sys)
elif version_info.major == 2:
    reload(sys)
    sys.setdefaultencoding("utf-8")

try: import simplejson as json
except: import json

# {"version":"1.0","properties":[{"id":"light_switch","name":"电灯开关","desc":"控制电灯开灭","required":true,"mode":"rw","define":{"type":"bool","mapping":{"0":"关","1":"开"}}},{"id":"color","name":"颜色","desc":"灯光颜色","mode":"rw","define":{"type":"enum","mapping":{"0":"Red","1":"Green","2":"Blue"}}},{"id":"brightness","name":"颜色","desc":"灯光颜色","mode":"rw","define":{"type":"int","unit":"%","unitDesc":"亮度百分比","min":"0","max":"100"}},{"id":"name","name":"灯位置名称","desc":"灯位置名称：书房、客厅等","mode":"rw","required":true,"define":{"type":"string","min":"0","max":"64"}}]}
# 数组类型最大数据长度
MAX_ARRAY_JSON_STR_LEN = (2048)
# 数组类型数组最大个数
MAX_SAMPLE_ARRAY_SIZE = (16)

class TEMPLATE_CONSTANTS:
    VERSION = "version"
    TYPE = "type"
    NAME = "name"
    ID = "id"
    MIN = "min"
    MAX = "max"
    DEFINE = "define"
    PROPERTIES = "properties"
    EVENTS = "events"
    MAPPING = "mapping"
    UNIT = "unit"
    UNITDESC = "unitDesc"
    REQUIRED = "required"
    MODE = "mode"
    SPECS = "specs"

class iot_enum:
    def __init__(self, parent, name, index):
        self.parent = parent
        self.id = name
        self.index = index

    def get_c_macro_name(self):
        return "e_{}_{}".format(self.parent.upper(), self.id.upper())

    def get_define_str(self):
        return "#define {} {}".format(self.get_c_macro_name(), self.index)

class iot_struct_field:
    def __init__(self, prefix, id, name, index, field_obj):
        self.default_value = ""
        self.enums = []
        self.index = index
        self.id = id
        self.prefix = prefix
        self.name = name
        self.type_name = field_obj["dataType"]["type"]

        if self.type_name == "bool":
            self.type_define = "TYPE_DEF_TEMPLATE_BOOL"
            self.type_id = "TYPE_TEMPLATE_BOOL"
            self.default_value = "0"
        elif self.type_name == "enum":
            self.type_define = "TYPE_DEF_TEMPLATE_ENUM"
            self.type_id = "TYPE_TEMPLATE_ENUM"
            #if TEMPLATE_CONSTANTS.DEFINE not in field_obj:
            #    raise ValueError("错误：结构体中 {} 字段定义中未找到枚举定义{} 字段".format(name, TEMPLATE_CONSTANTS.DEFINE))

            enum_defs = field_obj["dataType"]["mapping"]


            for enum_id in enum_defs:
                enum_name = enum_defs[enum_id]
                current_enum = iot_enum(self.id, enum_name, enum_id)
                self.enums.append(current_enum)
                if self.default_value == "":
                    self.default_value = enum_id
            if self.default_value == "":
                raise ValueError("错误：{} 字段默认值 {} 非法".format(name, field_obj["default"]))

        elif self.type_name == "float":
            self.type_define = "TYPE_DEF_TEMPLATE_FLOAT"
            self.type_id = "TYPE_TEMPLATE_FLOAT"

            self.min_value = field_obj["dataType"]["min"]
            self.max_value = field_obj["dataType"]["max"]
            self.default_value = field_obj["dataType"]["start"]
            if float(self.default_value) < float(self.min_value) or float(self.default_value) > float(self.max_value):
                raise ValueError("错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
        elif self.type_name == "int":
            self.type_define = "TYPE_DEF_TEMPLATE_INT"
            self.type_id = "TYPE_TEMPLATE_INT"

            self.min_value = field_obj["dataType"]["min"]
            self.max_value = field_obj["dataType"]["max"]
            self.default_value = field_obj["dataType"]["start"]
            if int(self.default_value) < int(self.min_value) or int(self.default_value) > int(self.max_value):
                raise ValueError("错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
        elif self.type_name == "string":
            self.type_define = "TYPE_DEF_TEMPLATE_STRING"
            self.type_id = "TYPE_TEMPLATE_STRING"

            self.min_value = field_obj["dataType"]["min"]
            self.max_value = field_obj["dataType"]["max"]
            self.default_value = "{'\\0'}"
        elif self.type_name == "timestamp":
            self.type_define = "TYPE_DEF_TEMPLATE_TIME"
            self.type_id = "TYPE_TEMPLATE_TIME"
            self.default_value = 0
        else:
            raise ValueError('{} 字段 数据类型 type={} 取值非法，有效值应为：bool,enum,int,float,string'.format(name, field_obj["type"]))

    def get_id_c_macro_name(self):
        return "TC_IOT_PROP_{}".format(self.id)

    def get_id_c_member_name(self):
        return "m_{}".format(self.id)

    def get_id_default_value(self):
        return "{}".format(self.default_value)

    def get_id_define_str(self):
        return "#define {} {}".format(self.get_id_c_macro_name(), self.index)

    def get_struct_field_declare(self):
        if self.type_id == "TYPE_TEMPLATE_STRING":
            return "TYPE_DEF_TEMPLATE_STRING m_{}[{}+1];".format(self.id, str(self.max_value))
        else:
            return "{} m_{};".format(self.type_define, self.id)

    def get_global_field_declare(self):
        if self.type_id == "TYPE_TEMPLATE_STRING":
            return "TYPE_DEF_TEMPLATE_STRING sg_{}{}[{}+1]={};".format(self.prefix, self.id, str(self.max_value),"{0}")
        else:
            return "{} sg_{}{} = {};".format(self.type_define, self.prefix, self.id, self.default_value)

    def get_meta_define_str(self, var_name):
        return '{{ "{}", &{}.{}, {} }},' \
                    .format(self.id, var_name, self.get_id_c_member_name(), self.type_id)

class iot_field:
    def __init__(self, prefix, id, name, mode, index, field_obj):
        self.default_value = ""
        self.enums = []
        self.index = index
        self.id = id
        self.prefix = prefix
        self.name = name
        self.type_name = field_obj["define"]["type"]
        self.fields = []
        self.field_id = 0
        self.mode = mode

        if self.type_name == "bool":
            self.type_define = "TYPE_DEF_TEMPLATE_BOOL"
            self.type_id = "TYPE_TEMPLATE_BOOL"
            self.default_value = "0"
        elif self.type_name == "enum":
            self.type_define = "TYPE_DEF_TEMPLATE_ENUM"
            self.type_id = "TYPE_TEMPLATE_ENUM"
            if TEMPLATE_CONSTANTS.DEFINE not in field_obj:
                raise ValueError("错误：{} 字段定义中未找到枚举定义{} 字段".format(name, TEMPLATE_CONSTANTS.DEFINE))

            enum_defs = field_obj["define"]["mapping"]

            for enum_id in enum_defs:
                enum_name = enum_defs[enum_id]
                current_enum = iot_enum(self.id, enum_name, enum_id)
                self.enums.append(current_enum)
                if self.default_value == "":
                    self.default_value = enum_id
            if self.default_value == "":
                raise ValueError("错误：{} 字段默认值 {} 非法".format(name, field_obj["default"]))

        elif self.type_name == "float":
            self.type_define = "TYPE_DEF_TEMPLATE_FLOAT"
            self.type_id = "TYPE_TEMPLATE_FLOAT"

            self.min_value = field_obj["define"]["min"]
            self.max_value = field_obj["define"]["max"]
            self.default_value = field_obj["define"]["start"]
            if float(self.default_value) < float(self.min_value) or float(self.default_value) > float(self.max_value):
                raise ValueError("错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
        elif self.type_name == "int":
            self.type_define = "TYPE_DEF_TEMPLATE_INT"
            self.type_id = "TYPE_TEMPLATE_INT"

            self.min_value = field_obj["define"]["min"]
            self.max_value = field_obj["define"]["max"]
            self.default_value = field_obj["define"]["start"]
            if int(self.default_value) < int(self.min_value) or int(self.default_value) > int(self.max_value):
                raise ValueError("错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
        elif self.type_name == "string":
            self.type_define = "TYPE_DEF_TEMPLATE_STRING"
            self.type_id = "TYPE_TEMPLATE_STRING"

            self.min_value = field_obj["define"]["min"]
            self.max_value = field_obj["define"]["max"]
            self.default_value = "{'\\0'}"
        elif self.type_name == "timestamp":
            self.type_define = "TYPE_DEF_TEMPLATE_TIME"
            self.type_id = "TYPE_TEMPLATE_TIME"
            self.default_value = 0
        elif self.type_name == "struct":
            self.type_define = "TYPE_DEF_TEMPLATE_OBJECT"
            self.type_id = "TYPE_TEMPLATE_JOBJECT"
            self.default_value = 0
            for field_define in field_obj["define"]["specs"]:
                if TEMPLATE_CONSTANTS.NAME not in field_define:
                    raise ValueError("错误：字段定义中未找到 Name 字段")
                self.fields.append(iot_struct_field("", field_define["id"], field_define["name"], self.field_id, field_define))
                self.field_id += 1
        elif self.type_name == "array":
            self.type_define = "TYPE_DEF_TEMPLATE_ARRAY"
            self.type_id = "TYPE_TEMPLATE_ARRAY"
            array_type = field_obj["define"]["arrayInfo"]["type"]
            self.array_info = {}
            self.array_info["type"] = array_type
            if(array_type == "string"):
                self.array_info["array_type"] = "TYPE_TEMPLATE_STRING"
                self.array_info["min_value"] = field_obj["define"]["arrayInfo"]["min"]
                self.array_info["max_value"] = field_obj["define"]["arrayInfo"]["max"]
                self.array_info["default_value"] = "{'\\0'}"
            elif array_type == "int":
                self.array_info["array_type"] = "TYPE_TEMPLATE_INT"
                self.array_info["min_value"] = field_obj["define"]["arrayInfo"]["min"]
                self.array_info["max_value"] = field_obj["define"]["arrayInfo"]["max"]
                self.array_info["default_value"] = field_obj["define"]["arrayInfo"]["start"]
                if int(self.array_info["default_value"]) < int(self.array_info["min_value"]) or int(self.array_info["default_value"]) > int(self.array_info["max_value"]):
                    raise ValueError(
                        "错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
            elif array_type == "struct":
                self.array_info["array_type"] = "TYPE_TEMPLATE_JOBJECT"
                self.array_info["default_value"] = self.get_id_struct_data_point_name(
                )
                if TEMPLATE_CONSTANTS.SPECS not in field_obj["define"]["arrayInfo"]:
                    raise ValueError("错误：字段定义中未找到 specs 字段")
                # self.array_info["struct_fields"] = []
                # self.array_info["struct_field_id"] = 0
                for member in field_obj["define"]["arrayInfo"]["specs"]:
                    # 使用define替代dataType，复用iot_field接口
                    member['define'] = member['dataType']
                    self.fields.append(iot_field("", member["id"], member["name"], field_obj["mode"], self.field_id, member))
                    self.field_id += 1
            elif array_type == "float":
                self.array_info["array_type"] = "TYPE_TEMPLATE_FLOAT"
                self.array_info["min_value"] = field_obj["define"]["arrayInfo"]["min"]
                self.array_info["max_value"] = field_obj["define"]["arrayInfo"]["max"]
                self.array_info["default_value"] = field_obj["define"]["arrayInfo"]["start"]
                if int(self.array_info["default_value"]) < int(self.array_info["min_value"]) or int(self.array_info["default_value"]) > int(self.array_info["max_value"]):
                    raise ValueError(
                        "错误：{} 字段 default 指定的默认值超出 min~max 取值范围".format(name))
        else:
            raise ValueError('{} 字段 数据类型 type={} 取值非法，有效值应为：bool,enum,int,float,string'.format(name, field_obj["type"]))

    def get_id_c_macro_name(self):
        return "TC_IOT_PROP_{}".format(self.id)

    def get_id_c_member_name(self):
        return "m_{}".format(self.id)

    def get_id_default_value(self):
        return "{}".format(self.default_value)

    def get_id_struct_data_point_name(self):
        return "sg_StructTemplate{}".format(self.id.title())

    def get_id_define_str(self):
        return "#define {} {}".format(self.get_id_c_macro_name(), self.index)

    def get_struct_field_declare(self):
        if self.type_id == "TYPE_TEMPLATE_STRING":
            return "TYPE_DEF_TEMPLATE_STRING m_{}[{}+1];".format(self.id, str(self.max_value))
        elif self.type_id == "TYPE_TEMPLATE_ARRAY":
            # 数组大小暂定为MAX_ARRAY_JSON_STR_LEN
            return "TYPE_DEF_TEMPLATE_ARRAY m_{}[MAX_ARRAY_JSON_STR_LEN +1];".format(self.id)
        elif self.type_id == "TYPE_TEMPLATE_JOBJECT":
            return "ivm_{} m_{};".format(self.id, self.id)
        else:
            return "{} m_{};".format(self.type_define, self.id)

    def get_global_field_declare(self):
        if self.type_id == "TYPE_TEMPLATE_STRING":
            return "TYPE_DEF_TEMPLATE_STRING sg_{}{}[{}+1]={};".format(self.prefix, self.id, str(self.max_value),"{0}")
        else:
            return "{} sg_{}{} = {};".format(self.type_define, self.prefix, self.id, self.default_value)

    def get_meta_define_str(self, var_name):
        return '{{ "{}", &{}.{}, {} }},' \
                    .format(self.id, var_name, self.get_id_c_member_name(), self.type_id)

    def gen_property_function(self, prefix):
        usr_func_str = ""
        #usr_func_str += "\nint iv_usrcb_ProWritable_{}(ivm_{} *property)".format(field.id, field.id)
        if self.type_id == "TYPE_TEMPLATE_JOBJECT":
            usr_func_str += "\nint {}_{}(const ivm_{} *{})".format(prefix, self.id, self.id, self.id)
        else:
            usr_func_str += "\nint {}_{}(const {} *{})".format(prefix, self.id, self.type_define, self.id)
        return usr_func_str

class iot_event:
    def __init__(self,id, name, index, event):
        self.index = index
        self.id = id
        self.name = name
        self.event_type = event["type"]
        self.desc = event["desc"]
        self.event_properties = []
        self.event_property_count = 0
        self.prefix = self.id + "_"

        for property in event["params"]:
            self.event_properties.append(iot_field(self.prefix, property["id"], property["name"], [], self.event_property_count, property))
            self.event_property_count += 1

    def get_sigle_event_info(self):
        event_info = ""
        event_info += "\n   id:{}  name:\"{}\" type:\"{}\"\n".format(self.id, self.name, self.event_type)
        event_info += "   property_count:{} \n   params:[".format(self.event_property_count)

        for field in self.event_properties:
            event_info += "\n      para:{}  type:{}".format(field.id, field.type_id)

        event_info += "\n   ]"
        return event_info

    def gen_sigle_event_info(self, event_id, global_name="g_ivm_objs"):
        result = ""
        event_para_info = ""
        event_property_info = ""
        event_var_info = ""
        struct_name = "Event"
        for field in self.event_properties:
            #event_para_info += "static {}\n".format(field.get_global_field_declare())
            event_property_info += "   {"
            if field.type_id == "TYPE_TEMPLATE_STRING":
                event_property_info += ".key = \"{}\", .data = {}.{}.m_{}.m_{}, .type = {}".format(field.id, global_name, struct_name, event_id, field.id, field.type_id)
            else:
                event_property_info += ".key = \"{}\", .data = &{}.{}.m_{}.m_{}, .type = {}".format(field.id, global_name,struct_name, event_id, field.id, field.type_id)
            event_property_info += "},\n"
        event_var_info += "static DeviceProperty g_propertyEvent_{}[] = ".format(self.id)
        result += event_para_info + event_var_info + "{\n"+event_property_info + "\n};\n"
        return result

    def gen_sigle_event_struct(self):
        result = "typedef struct {\n"
        for field in self.event_properties:
            if field.type_id == "TYPE_TEMPLATE_STRING":
                result += "    {} m_{}[{}+1];\n".format(field.type_define, field.id, str(field.max_value))
            else:
                result += "    {} m_{};\n".format(field.type_define, field.id)
        result += "} ivm_"+self.id+";\n\n"
        return result

class iot_action:
    def __init__(self,id, name, index, action):
        self.index = index
        self.id = id
        self.name = name
        self.desc = action["desc"]

        self.action_input = []
        self.action_input_count = 0
        self.action_input_prefix = self.id + "action_in"

        self.action_output = []
        self.action_output_count = 0
        self.action_output_prefix = self.id + "action_out"

        for input in action["input"]:
            self.action_input.append(iot_field(self.action_input_prefix, input["id"], input["name"], [], self.action_input_count, input))
            self.action_input_count += 1

        for output in action["output"]:
            self.action_output.append(iot_field(self.action_output_prefix, output["id"], output["name"], [], self.action_output_count, output))
            self.action_output_count += 1
        
    def get_single_action_info(self):
        action_info = ""
        action_info += "\n   id:{}  name:\"{}\"\n".format(self.id, self.name)

        action_info += "   action_input_count:{} \n   inputs:[".format(self.action_input_count)
        for field in self.action_input:
            action_info += "\n      para:{}  type:{}".format(field.id, field.type_id)
        action_info += "\n   ]"

        action_info += "   action_output_count:{} \n   output:[".format(self.action_output_count)
        for field in self.action_output:
            action_info += "\n      para:{}  type:{}".format(field.id, field.type_id)
        action_info += "\n   ]"

        return action_info

    def gen_single_action_info(self):
        result = ""
        action_para_info = ""
        action_input_info = ""
        action_input_var_info = ""

        if self.action_input_count + self.action_output_count > 0:
            action_para_info += "static ivm_{}_t sg_{};\n".format(self.id, self.id)

        for field in self.action_input:
            action_input_info += "   {"
            if field.type_id == "TYPE_TEMPLATE_STRING":
                action_input_info += ".key = \"{}\", .data_buff_len = sizeof(sg_{}.action_in.m_{}) - 1, .data = sg_{}.action_in.m_{}, .type = {}".format(field.id, self.id, field.id, self.id, field.id, field.type_id)
            else:
                action_input_info += ".key = \"{}\", .data_buff_len = 0, .data = &sg_{}.action_in.m_{}, .type = {}".format(field.id,self.id, field.id, field.type_id)
            action_input_info += "},"
        action_input_var_info += "static DeviceProperty g_actionInput_{}[] = ".format(self.id)
        result += action_para_info + action_input_var_info + "{\n"+action_input_info + "\n};\n"

        action_para_info = ""
        action_input_info = ""
        action_input_var_info = ""

        for field in self.action_output:
            action_input_info += "   {"
            if field.type_id == "TYPE_TEMPLATE_STRING":
                action_input_info += ".key = \"{}\", .data_buff_len = sizeof(sg_{}.action_out.m_{}) - 1, .data = sg_{}.action_out.m_{}, .type = {}".format(field.id, self.id, field.id, self.id, field.id, field.type_id)
            else:
                action_input_info += ".key = \"{}\", .data_buff_len = 0, .data = &sg_{}.action_out.m_{}, .type = {}".format(field.id, self.id, field.id, field.type_id)
            action_input_info += "},"
        action_input_var_info += "static DeviceProperty g_actionOutput_{}[] = ".format(self.id)
        result += action_para_info + action_input_var_info + "{\n"+action_input_info + "\n};\n"

        return result

    def gen_single_action_struct(self):
        result = ""

        if self.action_input_count + self.action_output_count > 0:
            result += "typedef struct {\n" 
            if self.action_input_count > 0:
                result += "    struct {\n"          
                for field in self.action_input:
                    if field.type_id == "TYPE_TEMPLATE_STRING":
                        result += "        {} m_{}[{}+1];\n".format(field.type_define, field.id, str(field.max_value))
                    else:
                        result += "        {} m_{};\n".format(field.type_define, field.id)
                result += "    }action_in;\n"

            if self.action_output_count > 0:
                result += "    struct {\n"       
                for field in self.action_output:
                    if field.type_id == "TYPE_TEMPLATE_STRING":
                        result += "        {} m_{}[{}+1];\n".format(field.type_define, field.id, str(field.max_value))
                    else:
                        result += "        {} m_{};\n".format(field.type_define, field.id)
                result += "    }action_out;\n"
            result += "}ivm_" + self.id + "_t;\n\n"
            
        return result

    def gen_single_action_function(self):
        result = ""
        result += "\nint iv_usrcb_Action_{}(".format(self.id)
        if self.action_input_count + self.action_output_count > 0:
            result += "ivm_{}_t *{})".format(self.id, self.id)
        else:
            result += "void *arg)"
        return result

class iot_struct:
    def __init__(self, model):
        self.version = model["version"]
        self.fields = []
        self.field_id = 0
        self.rfields = []
        self.rfield_id = 0
        
        self.events = []
        self.event_id = 0

        self.actions = []
        self.action_id = 0
        #self.filter = ['version_info', 'log_level', 'product_info']

        if  "properties" in model :
            for field_define in model["properties"]:
                if TEMPLATE_CONSTANTS.NAME not in field_define:
                    raise ValueError("错误：字段定义中未找到 Name 字段")
                if "_sys" not in field_define["id"] :
                    if  field_define["mode"] == "rw":
                        self.fields.append(iot_field("", field_define["id"], field_define["name"], field_define["mode"], self.field_id, field_define))
                        self.field_id += 1                   
                    else:
                        self.rfields.append(iot_field("", field_define["id"], field_define["name"], field_define["mode"], self.field_id, field_define))
                        self.rfield_id += 1

        if  "events" in model :
            for event in model["events"]:
                if TEMPLATE_CONSTANTS.NAME not in event:
                    raise ValueError("错误：字段定义中未找到 Name 字段")
                if "_sys" not in event["id"] :
                    self.events.append(iot_event(event["id"], event["name"], self.event_id, event))
                    self.event_id += 1

        if  "actions" in model :
            for action in model["actions"]:
                if TEMPLATE_CONSTANTS.NAME not in action:
                    raise ValueError("错误：字段定义中未找到 Name 字段")
                self.actions.append(iot_action(action["id"], action["name"], self.action_id, action))
                self.action_id += 1

    def dump_data_info(self):
        print("dump iot struct,counts:{}".format(self.field_id + self.rfield_id))
        for temp_field in self.fields + self.rfields:
            if temp_field.type_name == "enum":
                print("{} {} {} {} ".format(temp_field.id, temp_field.type_name, temp_field.default_value, temp_field.type_define))
                print("enums:{")
                for enum in temp_field.enums:
                    print("{}  ".format(enum.get_c_macro_name()))
                print("}")
            else:
                print("{} {} {} {}\n".format(temp_field.id, temp_field.type_name, temp_field.default_value,
                                             temp_field.type_define))

    def dump_event_info(self):
        count = 0
        event_str = ""
        event_str += ("#define EVENT_COUNTS     ({})\n").format(self.event_id)
        for event_d in self.events:
            event_str += "{}\n".format(event_d.gen_sigle_event_info())
            count += 1
        return event_str

    def data_config_macro_define(self, struct_Template="sDataPoint"):
        define_str = ""
        define_str += "/*-----------------data config start  -------------------*/ \n\n"
        
        for field in (self.fields + self.rfields):
            if field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":   
                define_str += "#define {}_MEMBER_COUNT {}\n".format(field.id.upper(), field.field_id)
                define_str += "static {}  sg_{}_DataTemplate[{}_MEMBER_COUNT];\n\n".format(struct_Template, field.id, field.id.upper())

        if self.field_id > 0:
            var_gTemplate = "sg_WR_DataTemplate"
            define_str += "#define TOTAL_WR_PROPERTY_COUNT {}\n\n".format(self.field_id)
            define_str += "static {}    {}[TOTAL_WR_PROPERTY_COUNT];\n\n".format(struct_Template, var_gTemplate)
        if self.rfield_id > 0:
            var_gTemplate = "sg_RO_DataTemplate"
            define_str += "#define TOTAL_RO_PROPERTY_COUNT {}\n\n".format(self.rfield_id)
            define_str += "static {}    {}[TOTAL_RO_PROPERTY_COUNT];\n\n".format(struct_Template, var_gTemplate)
        if (self.field_id + self.rfield_id + self.event_id) > 0:
            struct_Template = "ivm_extend_param_t"
            var_gTemplate = "g_ivm_objs"
            define_str += "{}    {};\n\n".format(struct_Template, var_gTemplate)
        return define_str

    def declare_product_data_struct_2(self, struct_field):
        struct_name = struct_field.id
        var_gname = "sg_"
        var_gname += struct_field.id
        struct = ""
        result = ""
        result += "typedef struct {\n"
        for field in struct_field.fields:
            if field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":
                struct += declare_product_data_struct_2(field)
            result += "    {}\n".format(field.get_struct_field_declare())
        result += "} ivm_" + struct_name + ";\n\n"
        struct += result
        return struct

    def declare_product_data_struct(self, struct_name="ivm_extend_param_t", var_gname="g_ivm_objs"):
        struct = ""
        result = ""
        result += "typedef struct {\n"

        if self.action_id > 0:
            for action_d in self.actions:
                struct += action_d.gen_single_action_struct()

        if self.rfield_id > 0:
            result += "    struct {\n"
            for field in self.rfields:
                if field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":           
                    struct += self.declare_product_data_struct_2(field)
                result += "        {}\n".format(field.get_struct_field_declare())
            result += "    } ProReadonly;\n"

        if self.field_id > 0:
            result += "    struct {\n"
            for field in self.fields:
                if field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":           
                    struct += self.declare_product_data_struct_2(field)
                if field.type_define == "TYPE_DEF_TEMPLATE_ARRAY":
                    if field.array_info["array_type"] == "TYPE_TEMPLATE_JOBJECT":
                        struct += self.declare_product_data_struct_2(field)
                result += "        {}\n".format(field.get_struct_field_declare())
            result += "    } ProWritable;\n"

        if self.event_id > 0:
            result += "    struct {\n"
            for event_d in self.events:
                struct += event_d.gen_sigle_event_struct()
                result += "        ivm_{} m_{};\n".format(event_d.id, event_d.id)
            result += "    } Event;\n"

        result += "} " + struct_name + ";\n\n"
        result += "extern   " + struct_name + "     "+var_gname + ";\n\n"
        struct += result
        return struct

    def declare_product_readonly_macro_define(self, global_name="g_ivm_objs"):
        result =""
        result += """/*!    \\brief	物模型实例全局变量,应用层可直接访问/修改这个对象的成员变量。
        注意:
        1. 设备开发者可直接访问{}全局变量访问/修改物模型
        如：对一个int32类型的物模型对象yyyy的状态置值，应用层可直接操作：
        ```
        ivm_lock();		//互斥加锁
        {}.ProReadonly.yyyy = 1;		       //置状态值
        iv_dm_property_report(yyy, cb, param); //置状态
        ivm_unlock();	//互斥解锁
        ```
        2. 访问互斥
        * 在iv_usrcb_xxxxx系列回调函数中访问时{}时，无需加锁
        * 在其它任何地方访问{}，都应该调用ivm_lock()/ivm_unlock()作互斥处理
        3. 不得在iv_usrcb_xxxxx系列回调函数进行导致阻塞或耗时的操作，这样会阻塞核心通讯线程.
        4. 读写表示该属性即可从设备端上报到云端也可从云端发起控制，只读表示该属性只从设备向云端上报。
        */\n""" .format(global_name, global_name, global_name, global_name)

        result += "/*! \n \\brief  更新物模型 --整形\n */ \n"
        result += "#define ivm_ProReadonly_setInt(obj, _val) \\\n"
        result += "   do {                                   \\\n" 
        result += "       ivm_lock();                        \\\n"
        result += "       {}.ProReadonly.m_##obj = _val; \\\n" .format(global_name)
        result += "       iv_dm_property_report(#obj, NULL, NULL);\\\n"
        result += "       ivm_unlock();                      \\\n"
        result += "   } while (0)\n\n"
        
        result += "#define ivm_ProWritable_setInt(obj, _val) \\\n"
        result += "   do {                                   \\\n" 
        result += "       ivm_lock();                        \\\n"
        result += "       {}.ProWritable.m_##obj = _val; \\\n" .format(global_name)
        result += "       iv_dm_property_report(#obj, NULL, NULL);\\\n"
        result += "       ivm_unlock();                      \\\n"
        result += "   } while (0)\n"

        result += "/*! \n \\brief  更新物模型 --字符串\n */ \n"
        result += "#define ivm_ProReadonly_setString(obj, _str)  \\\n"
        result += "   do {                                   \\\n" 
        result += "       ivm_lock();                        \\\n"
        result += "       strncpy({}.ProReadonly.m_##obj, _str, sizeof({}.ProReadonly.m_##obj) - 1); \\\n" .format(global_name, global_name)
        result += "       iv_dm_property_report(#obj, NULL, NULL);       \\\n"
        result += "       ivm_unlock();                      \\\n"
        result += "   } while (0)\n\n"

        result += "#define ivm_ProWritable_setString(obj, _str)  \\\n"
        result += "   do {                                   \\\n" 
        result += "       ivm_lock();                        \\\n"
        result += "       strncpy({}.ProWritable.m_##obj, _str, sizeof({}.ProWritable.m_##obj) - 1); \\\n" .format(global_name, global_name)
        result += "       iv_dm_property_report(#obj, NULL, NULL);       \\\n"
        result += "       ivm_unlock();                      \\\n"
        result += "   } while (0)\n"

        result += "/*! \n \\brief  更新结构体--字符串成员\n */ \n"
        result += "#define ivm_ProWritable_setStructString(obj, member, _str)  \\\n"
        result += "   do {                                   \\\n" 
        result += "       strncpy({}.ProWritable.m_##obj.m_##member, _str, sizeof({}.ProWritable.m_##obj.m_##member) - 1); \\\n" .format(global_name, global_name)
        result += "   } while (0)\n"
        return result

    def property_data_initializer_2(self, struct_field, val_ctl, var_gTemplate):
        count = 0
        init_str = ""
        struct_name = "m_" + struct_field.id
        var_gProduct = "sg_" + struct_field.id + "_DataTemplate" 

        for field in struct_field.fields:
            if field.type_define == "TYPE_DEF_TEMPLATE_STRING":
                init_str += "    {}[{}].data_property.data = {}.{}.{}.{};\n".format(var_gProduct, count, var_gTemplate, val_ctl, struct_name, field.get_id_c_member_name())
                init_str += "    {}[{}].data_property.data_buff_len = sizeof({}.{}.{}.{})/sizeof({}.{}.{}.{}[0]);\n"\
                .format(var_gProduct, count, var_gTemplate, val_ctl, struct_name, field.get_id_c_member_name(),  var_gTemplate, val_ctl, struct_name, field.get_id_c_member_name())
            elif field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":
                init_str += "    {}.{} = {};\n".format(var_gProduct, field.get_id_c_member_name(), "NULL")
                init_str += "    {}[{}].data_property.data = &sg_DataTemplate_{};\n".format(var_gTemplate, count, field.id)
                init_str += "    {}[{}].data_property.data_buff_len = TOTAL_PROPERTY_COUNT_{};\n".format(var_gTemplate, count, field.id)
            else:
                init_str += "    {}[{}].data_property.data = &{}.{}.{}.{};\n".format(var_gProduct, count, var_gTemplate, val_ctl, struct_name, field.get_id_c_member_name())
            init_str += "    {}[{}].data_property.key  = \"{}\";\n".format(var_gProduct, count, field.id)
            init_str += "    {}[{}].data_property.type = {};\n".format(var_gProduct, count, field.type_id)
            init_str += "    {}[{}].state = eNOCHANGE;\n\n".format(var_gProduct, count)
            count += 1
        return init_str

    def property_data_initializer(self, var_gTemplate = "g_ivm_objs"):
        count = 0
        struct_str = ""
        init_str = ""
        init_str += "static void _ivm_init_data_template(void)\n{\n"
          
        var_gProduct="sg_RO_DataTemplate"
        for field in self.rfields:
            if field.type_define == "TYPE_DEF_TEMPLATE_STRING":
                init_str += "    {}[{}].data_property.data = {}.ProReadonly.{};\n".format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name())
                init_str += "    {}[{}].data_property.data_buff_len = sizeof({}.ProReadonly.{})/sizeof({}.ProReadonly.{}[{}]);\n"\
                .format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name(),var_gTemplate, field.get_id_c_member_name(), count)
            elif field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":
                init_str += "    {}[{}].data_property.data = sg_{}_DataTemplate;\n".format(var_gProduct, count, field.id)
                init_str += "    {}[{}].data_property.struct_obj_num = {}_MEMBER_COUNT;\n".format(var_gProduct, count, field.id.upper())
                struct_str += self.property_data_initializer_2(field, "ProReadonly", var_gTemplate)
            else:
                init_str += "    {}[{}].data_property.data = &{}.ProReadonly.{};\n".format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name())
            init_str += "    {}[{}].data_property.key  = \"{}\";\n".format(var_gProduct, count, field.id)
            init_str += "    {}[{}].data_property.type = {};\n".format(var_gProduct, count, field.type_id)
            init_str += "    {}[{}].state = eNOCHANGE;\n\n".format(var_gProduct, count)
            count += 1

        count = 0
        var_gProduct="sg_WR_DataTemplate"
        for field in self.fields:
            if field.type_define == "TYPE_DEF_TEMPLATE_STRING":
                init_str += "    {}[{}].data_property.data = {}.ProWritable.{};\n".format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name())
                init_str += "    {}[{}].data_property.data_buff_len = sizeof({}.ProWritable.{})/sizeof({}.ProWritable.{}[{}]);\n"\
                .format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name(),var_gTemplate, field.get_id_c_member_name(), count)
            elif field.type_define == "TYPE_DEF_TEMPLATE_OBJECT":
                init_str += "    {}[{}].data_property.data = sg_{}_DataTemplate;\n".format(var_gProduct, count, field.id)
                init_str += "    {}[{}].data_property.data_buff_len = {}_MEMBER_COUNT;\n".format(var_gProduct, count, field.id.upper())
                struct_str += self.property_data_initializer_2(field, "ProWritable", var_gTemplate)
            elif field.type_define == "TYPE_DEF_TEMPLATE_ARRAY":
                # if field.array_info["array_type"] == "TYPE_TEMPLATE_JOBJECT":
                #     init_str += "    {}();\n".format(field.get_id_struct_init_function_name())
                init_str += "    memset({}.{}, 0, sizeof({}.{}));\n".format(
                    "&" + var_gTemplate + ".ProWritable", field.get_id_c_member_name(), var_gTemplate + ".ProWritable", field.get_id_c_member_name())
                init_str += "    {}[{}].data_property.data = {}.{};\n".format(
                    var_gProduct, count, "&" + var_gTemplate + ".ProWritable", field.get_id_c_member_name())
                init_str += "    {}[{}].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;\n".format(
                    var_gProduct, count
                )
                init_str += "    {}[{}].data_property.array_info.array_type = {};\n".format(
                    var_gProduct, count, field.array_info["array_type"]
                )

            else:
                init_str += "    {}[{}].data_property.data = &{}.ProWritable.{};\n".format(var_gProduct, count, var_gTemplate, field.get_id_c_member_name())
            init_str += "    {}[{}].data_property.key  = \"{}\";\n".format(var_gProduct, count, field.id)
            init_str += "    {}[{}].data_property.type = {};\n".format(var_gProduct, count, field.type_id)
            init_str += "    {}[{}].state = eNOCHANGE;\n\n".format(var_gProduct, count)
            count += 1

        init_str += struct_str
        init_str += "};\n"
        return init_str

    def gen_data_declare(self):
        data_config = ""
        data_config +="{}".format(self.declare_product_data_struct())
        data_config +="{}".format(self.declare_product_readonly_macro_define())
        return data_config

    def gen_data_config(self):
        data_config = ""
        data_config +="{}".format(self.data_config_macro_define())
        #data_config +="{}".format(self.declare_product_data_struct())
        data_config += "{}".format(self.property_data_initializer())
        return data_config

    def gen_event_config(self):
        result = ""
        event_config = ""
        events_var = ""
        event_str = ""

        event_config += ("\n#define EVENT_COUNTS     ({})\n\n").format(self.event_id)
        events_var += "\nstatic sEvent g_events[]={"
        for event_d in self.events:
            event_config += "{}".format(event_d.gen_sigle_event_info(event_d.id))
            event_str += "\n    {"
            event_str += "\n     .event_name = \"{}\",".format(event_d.id)
            event_str += "\n     .type = \"{}\",".format(event_d.event_type)
            event_str += "\n     .timestamp = 0,"
            event_str += "\n     .eventDataNum = {},".format(event_d.event_property_count)
            event_str += "\n     .pEventData = g_propertyEvent_{},".format(event_d.id)
            event_str +="\n    },"
        result += event_config + events_var + event_str + "\n};\n"
        return result
    
    def gen_action_config(self):
        result = ""
        action_config = ""
        actions_var = ""
        action_str = ""

        action_config += ("\n#define TOTAL_ACTION_COUNTS     ({})\n\n").format(self.action_id)
        actions_var += "\nstatic DeviceAction g_actions[]={"
        for action_d in self.actions:
            action_config += "{}".format(action_d.gen_single_action_info())
            action_str += "\n    {"
            action_str += "\n     .pActionId = \"{}\",".format(action_d.id)
            action_str += "\n     .timestamp = 0,"
            action_str += "\n     .input_num = {},".format(action_d.action_input_count)
            action_str += "\n     .output_num = {},".format(action_d.action_output_count)
            action_str += "\n     .pInput = g_actionInput_{},".format(action_d.id)
            action_str += "\n     .pOutput = g_actionOutput_{},".format(action_d.id)
            action_str +="\n    },"
        result += action_config + actions_var + action_str + "\n};\n"
        return result

    def gen_usrex_func(self):
        result = ""
        usr_func_comment = ""
        usr_func_header = ""
        usr_func_str = ""
        usr_func_dgb = ""

        usr_func_comment += "/*"
        usr_func_comment += "\n    用户实现本文件中定义的各个功能回调函数，即实现了具体的业务功能."
        usr_func_comment += "\n    回调函数实现注意事项:\n"
        usr_func_comment += "\n    1.在回调函数中，不能做阻塞式操作，禁止在回调函数中做耗时的操作;"
        usr_func_comment += "\n    2.在非回调函数中访问物模型对象资源，应该使用ivm_lock()/ivm_unlock()加锁和解锁;"
        usr_func_comment += "\n    3.在回调函数处理中，不要进行加锁/解锁操作。（因为通讯线程已经加锁了）\n*/"
        usr_func_comment += "\n"

        usr_func_header += "\n#include <assert.h>"
        #usr_func_header += "\n#include \"qcloud_iot_export.h\""
        usr_func_header += "\n#include \"iv_usrex.h\""
        usr_func_header += "\n#include \"iv_cm.h\""
        usr_func_header += "\n"

        #usr_func_dgb += "\nvoid property_dbg_info(DeviceProperty *property)\n{"
        #usr_func_dgb += "\n    if (property->type == JBOOL) {Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JINT32){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JINT16){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JINT8){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JUINT32){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JUINT16){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JUINT8){Log_d(\"val:%d\", *(int *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JFLOAT){Log_d(\"val:%f\", *(float *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JDOUBLE){Log_d(\"val:%f\", *(double *)property->data);}"
        #usr_func_dgb += "\n    else if (property->type == JSTRING){ Log_d(\"val:%s\", (char *)property->data);}"
        #usr_func_dgb += "\n}\n"

        #usr_func_dgb += "\nvoid action_dbg_info(DeviceAction *action)\n{"
        #usr_func_dgb += "\n    int i = 0;"
        #usr_func_dgb += "\n    Log_d(\"time:%d, id:%s, input:%d, output:%d\", action->timestamp, action->pActionId, action->input_num, action->output_num);"
        #usr_func_dgb += "\n    for (i = 0; i < action->input_num; i++){property_dbg_info(&action->pInput[i]);}"
        #usr_func_dgb += "\n    for (i = 0; i < action->output_num; i++){property_dbg_info(&action->pOutput[i]);}"
        #usr_func_dgb += "\n}\n"

        for field in self.fields:
            usr_func_str += "\n{}".format(field.gen_property_function("iv_usrcb_ProWritable"))
            usr_func_str += "\n{"
            #usr_func_str += "\n    property_dbg_info(property);"
            usr_func_str += "\n    //User implementation code"
            usr_func_str += "\n    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!"
            usr_func_str += "\n    return 0;"
            usr_func_str += "\n}"
            usr_func_str += "\n"

        for action_d in self.actions:
            usr_func_str += "\n{}".format(action_d.gen_single_action_function())
            usr_func_str += "\n{"
            #usr_func_str += "\n    action_dbg_info(action);"
            usr_func_str += "\n    //User implementation code"
            usr_func_str += "\n    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!"
            usr_func_str += "\n    return 0;"
            usr_func_str += "\n}"
            usr_func_str += "\n"

        result += usr_func_comment + usr_func_header + usr_func_dgb + usr_func_str + "\n"
        return result

    def gen_usrexh(self):
        result = ""
        usrexh_header = ""
        usrexh_commment = ""
        define_str = ""
        usrexh_func = ""

        usrexh_header += "\n#include \"qcloud_iot_export.h\""
        usrexh_header += "\n#include \"iv_def.h\""
        usrexh_header += "\n#include \"iv_dm.h\"\n\n"
        usrexh_header += "#ifdef __cplusplus\nextern \"C\"\n{\n#endif\n\n"

        usrexh_commment += "/*\n以下函数由开发者应用层去实现，实现具体的业务功能:\n"\
                         "1. 云平台在接收到对设备的设置操作(更改ProWritable)或控制操作(Action)时,会主动通知设备;\n"\
                         "2. 设备SDK通过iv_usrcb_ProWritable_xxx/iv_usrcb_Action_xxx系列函数通知应用层;\n"\
                         "3. 函数的参数，即为对应物模型对象的指针;\n*/\n"

        define_str += "#define MAX_ARRAY_JSON_STR_LEN ({})\n".format(
            MAX_ARRAY_JSON_STR_LEN)

        if (self.rfield_id + self.field_id) > 0:
            usrexh_func += self.gen_data_declare()
        #for field in self.rfields:
        #    usrexh_func += "\n\nint iv_usrcb_ProReadonly_{}(DeviceProperty *property);".format(field.id)
        for field in self.fields:
            #usrexh_func += "\nint iv_usrcb_ProWritable_{}(DeviceProperty *property);".format(field.id)
            usrexh_func += "{};".format(field.gen_property_function("iv_usrcb_ProWritable"))
        usrexh_func += "\n"
        for action_d in self.actions:
            usrexh_func += "{};".format(action_d.gen_single_action_function())
        usrexh_func += "\n"

        usrexh_func += "\n#ifdef __cplusplus\n}\n#endif\n"
        
        result += usrexh_header + usrexh_commment + define_str + usrexh_func + "\n"

        return result

    def gen_usrex(self):
            result = ""
            usrex_header = ""
            usrex_extern = ""
            usrex_func = ""

            usrex_header += "#include \"iv_usrex.h\"\n#include \"iv_def.h\"\n"
            if (self.field_id + self.rfield_id) != 0:
                usrex_header += self.gen_data_config()
            if self.action_id != 0:
                usrex_header += self.gen_action_config()
            if self.event_id != 0:
                usrex_header += self.gen_event_config()
            usrex_header += "\n"

            if  self.action_id != 0:
                usrex_extern += "extern int ivm_init_Action(void *obj, int num, char *name, ivm_callback_Action cb);\n"
            if  self.field_id != 0:             
                usrex_extern += "extern int ivm_init_ProWritable(void *obj, int num, char *name, ivm_callback_ProWritable cb, char close_sync);\n"
            if  self.rfield_id != 0:
                usrex_extern += "extern int ivm_init_ProReadonly(void *obj, int num, char *name);\n"
            if  self.event_id != 0:
                usrex_extern += "extern int ivm_init_Event(void *obj, int num, char *name);\n"
            
            usrex_extern += "\n"
            
            if self.rfield_id != 0:
                usrex_extern += "#define ivm_doi_init_ProReadonly(data) ivm_init_ProReadonly(sg_RO_DataTemplate, TOTAL_RO_PROPERTY_COUNT, #data)\n"               
            if  self.field_id != 0:
                usrex_extern += "#define ivm_doi_init_ProWritable(data, flag) "\
                              "ivm_init_ProWritable(sg_WR_DataTemplate, TOTAL_WR_PROPERTY_COUNT, #data, "\
                              "(ivm_callback_ProWritable)iv_usrcb_ProWritable_##data, flag)\n"
            if  self.action_id != 0:
                usrex_extern += "#define ivm_doi_init_Action(data)  "\
                              "ivm_init_Action(g_actions, TOTAL_ACTION_COUNTS, #data, (ivm_callback_Action)iv_usrcb_Action_##data)\n"
            if  self.event_id != 0:
                usrex_extern += "#define ivm_doi_init_Event(data) ivm_init_Event(g_events, EVENT_COUNTS, #data)\n"

            usrex_extern += "\n"

            usrex_func += "\nint ivm_env_init(void)\n{\n"
            usrex_func += "    memset((void *) &g_ivm_objs, 0, sizeof(ivm_extend_param_t));\n"
            if  (self.field_id + self.rfield_id)!= 0:
                usrex_func += "    _ivm_init_data_template();\n\n"
            for field_d in self.rfields:                
                usrex_func += "    ivm_doi_init_ProReadonly({});\n".format(field_d.id)
            for field_d in self.fields:                
                usrex_func += "    ivm_doi_init_ProWritable({}, 0);\n".format(field_d.id)
            usrex_func += "\n"
            for action_d in self.actions:                
                usrex_func += "    ivm_doi_init_Action({});\n".format(action_d.id)
            usrex_func += "\n"
            for events_d in self.events:                
                usrex_func += "    ivm_doi_init_Event({});\n".format(events_d.id)
            usrex_func += "\n"
            usrex_func += "    return 0;"
            result += usrex_header + usrex_extern + usrex_func + "\n}\n"
            return result

def main():
    parser = argparse.ArgumentParser(description='Iothub datatemplate and events config code generator.', usage='use "./codegen.py -c xx/config.json" gen config code')
    parser.add_argument('-c','--config', dest='config',metavar='xxx.json', required=False,default='xxx.json',
                        help='copy the generated file (data_config.c and events_config.c) to datatemplate_sample dir '
                             'or your own code dir with datatemplate. '
                              '\nconfig file can be download from tencent iot-hub platfrom. https://console.cloud.tencent.com/iotcloud')
    parser.add_argument('-d','--dest', dest='dest', required=False,default='.',
                        help='Dest directory for generated code files, no / at the end.')

    args = parser.parse_args()

    config_path = args.config
    if not os.path.exists(config_path):
        print(u"错误：配置文件不存在，请重新指定数据模板配置文件路径,请参考用法 ./codegen.py -c xx/data_template.json".format(config_path))
        return 1

    config_dir = os.path.dirname(config_path)
    if config_dir:
        config_dir += "/"
    
    # f = open(config_path, "r", encoding='GBK')
    f = open(config_path, "r")
    
    try:
        thingmodel = json.load(f)
        if 'properties' not in thingmodel:
            thingmodel.properties = []

        if 'events' not in thingmodel:
            thingmodel.events = []

        print(u"加载 {} 文件成功".format(config_path))
    except ValueError as e:
        print(u"错误：文件格式非法，请检查 {} 文件是否是 JSON 格式。".format(config_path))
        return 1

    if TEMPLATE_CONSTANTS.PROPERTIES not in thingmodel:
        print(u"错误：{} 文件中未发现 DataTemplate 属性字段，请检查文件格式是否合法。".format(config_path))
        return 1

    try:
        snippet = iot_struct(thingmodel)

        if (snippet.action_id + snippet.field_id + snippet.rfield_id + snippet.event_id) != 0:
            output_usrex_func_file_name = args.dest + "/iv_usrex_func.c"
            output_file = open(output_usrex_func_file_name, "w")
            output_file.write("{}".format(snippet.gen_usrex_func()))
            output_file.close()
            print(u"文件 {} 生成成功".format(output_usrex_func_file_name))

        if (snippet.action_id + snippet.field_id + snippet.rfield_id + snippet.event_id) != 0:
            output_usrexh_file_name = args.dest + "/iv_usrex.h"
            output_file = open(output_usrexh_file_name, "w")
            output_file.write("#ifndef __IVM_USREX_H_\n#define __IVM_USREX_H_\n{}\n#endif\n".format(snippet.gen_usrexh()))
            output_file.close()
            print(u"文件 {} 生成成功".format(output_usrexh_file_name))

        if (snippet.action_id + snippet.field_id + snippet.rfield_id + snippet.event_id) != 0:
            output_usrex_file_name = args.dest + "/iv_usrex.c"
            output_file = open(output_usrex_file_name, "w")
            output_file.write("{}".format(snippet.gen_usrex()))
            output_file.close()
            print(u"文件 {} 生成成功".format(output_usrex_file_name))
        return 0

    except ValueError as e:
        print(e)
        return 1

if __name__ == '__main__':
    sys.exit(main())
