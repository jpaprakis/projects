from pycparser import parse_file, c_ast, c_generator, plyparser

class RegularTypeDeclaration(object):
    """ Handling a non-struct declaration node object
    """
    def __init__(self, node, outer, location_info, struct_name_val=""):
        self.node = node
        self.location_info = location_info
        self.outer = outer
        self.type_of_var = self.outer.get_actual_type(self.node) #just renaming this for consistency
        self.ptr_depth = 0
        self.var_typerep = self.outer.primitive_types.get(self.type_of_var)
        self.var_name_val = struct_name_val + self.node.name
        self.var_new_val = True
        self.is_uninit = self.check_if_init()
        self.is_funcdecl = self.check_if_funcdecl()

    def check_if_init(self):
        if self.node.init == None:
            return True
        else:
            return False

    def check_if_funcdecl(self):
        return (isinstance(self.node.init, c_ast.FuncCall) and ((self.outer.get_funccall_funcname(self.node) in outer.func_list)))

    def create_printf_node(self):
        add_id = c_ast.ID('printf')
        add_id_addr = None
        add_id_val = None
        add_id_size = None
        add_return_val = None
        add_id_ptr_size = None
        add_id_hex = None
        line_to_add = self.location_info.line_no
        
        line_no = "line:"+ (str)(line_to_add)
        function = (str)(self.outer.item_delimiter) +"function:"+ (str)(self.location_info.func_name)


        var_info = ""
        var_name = (str)(self.outer.item_delimiter) +"var_name:"+ (str)(self.var_name_val)
        add_id_addr = c_ast.ID('&(' + self.var_name_val+')')

        var_addr = (str)(self.outer.item_delimiter) +"addr:%p"
        var_type = (str)(self.outer.item_delimiter) +"type:"+ (str)(self.type_of_var)
        var_new = (str)(self.outer.item_delimiter) +"new:"+ (str)(self.var_new_val)

        #If on the stack, size is just the sizeof the variable name and location is stack
        location_info = "stack"

        var_location = (str)(self.outer.item_delimiter) +"location:"+location_info

        var_uninitialized = (str)(self.outer.item_delimiter) +"uninitialized:" + (str)(self.is_uninit)

        var_size = (str)(self.outer.item_delimiter) +"max_size:%lu"
        var_hex = (str)(self.outer.item_delimiter) +"hex_value:%lX"
        value_type_set = (str)(self.var_typerep)

        var_val = (str)(self.outer.item_delimiter) +"value:" + value_type_set

        var_free = ""
        is_global = ""
        var_isarray = ""
        var_is_ptr = ""
        var_ptr_size = ""
        returning_func = ""

        var_info = var_name + var_addr +var_type + var_new + var_hex + var_isarray +is_global +var_location +var_uninitialized + var_free+var_size + var_is_ptr + var_ptr_size + var_val

        add_id_val = c_ast.ID(self.var_name_val)
        add_id_hex = c_ast.ID(self.var_name_val)
        
        add_id_size = c_ast.ID('(unsigned long)(sizeof(' + self.var_name_val+'))')
 
        var_dict_add = {(str)(self.var_name_val):(str)(self.type_of_var)}
        self.outer.var_type_dict.update(var_dict_add)

        #Finished changed variable block
        str_to_add = (str)(self.outer.print_wrapper) + line_no + function + var_info 
        add_const = c_ast.Constant('string', '"'+str_to_add+'"')

        all_items_array = [add_const, add_id_addr, add_id_hex, add_id_size, add_id_ptr_size, add_id_val, add_return_val]
        exprlist_array = []
        for item in all_items_array:
            if item != None:
                exprlist_array.append(item)
        add_exprList = c_ast.ExprList(exprlist_array)

        new_node = c_ast.FuncCall(add_id, add_exprList)
        
        return new_node


class StructTypeDeclaration(object):
    """ Handling a struct declaration node object
    """
    def __init__(self, parent):
        TypeDeclaration.__init__(self, parent)

#TODO: change this later as it is a subclass of many different types of parent nodes
class InsideFuncCall(object):
    """ Handling nodes that are being assigned to return values of functions inside our program
    """
    def __init__(self, parent):
        RegularTypeDeclaration.__init__(self, parent)

#TODO: change this later as it is a subclass of many different types of parent nodes
class PrintNode(object):
    """ Handling nodes that are being assigned to return values of functions inside our program
    """
    def __init__(self, node):
        self.print_node = node.create_printf_node()

class LocationInfo(object):
    """ Info on how many nodes we have added before and after the current node we are examining.
        Also contains information on the specific parent array that we are adding to, as well as
        which index of that parent we are adding to, and the function it's in
    """

    #on initialization, parent[initial_index] would give us the current node we're examining. Once 
    #we add more nodes in front of it, we can access it by getting parent[initial_index+added_before]
    def __init__(self, parent, initial_index, func_name):
        self.parent = parent
        self.initial_index = initial_index
        self.added_before = 0
        self.added_after = 0 #self.amt_after
        self.index_to_add = None
        self.additional_space = 0  #self.extra_after
        self.func_name = func_name
        self.line_no = parent[initial_index].coord.line

