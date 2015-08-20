from languages.c.visualizer.cg_stacktrace_functions import *
import problems_c.models
import logging
import pdb
import uuid
import sys
import os
import datetime
sys.path.extend(['.', '..'])
from pycparser import parse_file, c_ast, c_generator, plyparser
from languages.c.visualizer.cg_stacktrace_objects import *


#to_add_index will contain any extra amount we need to add to the index from node insertions in front of the current node, added by
#other functions
to_add_index= 0

class CVisualizerRefactor:

    def __init__(self, user, temp_path):
        self.primitive_types = \
        {'char':'%c',
         'signed char':'%c',
         'unsigned char':'%c',
         'short':'%d',
         'short int': '%d',
         'signed short': '%d',
         'signed short int': '%c',
         'unsigned short': '%u',
         'unsigned short int': '%u',
         'int': '%d',
         'int *': '%p',
         'signed int': '%d',
         'unsigned': '%u',
         'unsigned int': '%u',
         'long': '%ld',
         'long int': '%ld',
         'signed long': '%ld',
         'signed long int': '%ld',
         'unsigned long': '%lu',
         'unsigned long int': '%lu',
         'long long': '%lld',
         'long long int': '%lld',
         'signed long long': '%lld',
         'signed long long int': '%lld',
         'unsigned long long': '%llu',
         'unsigned long long int': '%llu',
         'float': '%f',
         'double': '%f',
         'long double': '%lf',
         'void *': '%p',
         'void': 'no',
         'string': '%s',
         'char *': '%s'}

        self.user = user
        self.temp_path = temp_path
        self.date_time = str((datetime.datetime.now() - datetime.datetime(1970, 1, 1)).total_seconds())

        #List if removed lines from the start of the c program, to be put back in later
        self.removed_lines = []

        """Global hash variables, these are what we'll use to denote different parts of our added print statements"""

        #print_wrapper will hold the pattern we'll use to identify where our print statements begin and end
        self.print_wrapper = uuid.uuid4().hex

        #item_delimiter will hold the pattern we'll use to identify where different items in a single print statement begin and end
        self.item_delimiter = uuid.uuid4().hex

        #malloc_size_var_name is the name of the variable we'll be using to keep track of the size of any mallocs in the c code
        #We will just declare it as global to start
        self.malloc_size_var_name = "malloc_size"+(str)(uuid.uuid4().hex)

        #Delimiter to keep track of where array hex value begins
        self.array_hex_delim = uuid.uuid4().hex

        """Dictionaries and lists required to keep track of specific things we've come across in the AST"""
        #func_list keeps track of all the function declaractions in our program and their type
        self.func_list = {}

        #var_type_dict will hold a dictionary of all the variables we've seen, and their types - used for return val printing
        self.var_type_dict = {}

        #ptr_dict will hold a dictionary of all the pointer names we've seen, and the amt of levels of pointers
        #ie: int **ptr_name would be {ptr_name:2}
        self.ptr_dict = {}

        #amt_after keeps track of the amount of print nodes we just added after the current node, used for returns
        self.amt_after = 0

        #global_print_nodes will contain a list of ast print nodes that we must insert at the beginning of the main function. We come
        #across these through global declarations, but can only insert them in main
        self.global_print_nodes = []

        #array_dict will hold the name as the key, with type, size nodes of each level, temporary int vars to be used in for loops,and depth as values in a list
        #{array_name:[type, [size node1, size node2], [temp for var 1, temp for var 2],depth, ptr_depth]}
        #int x[3][4]; would show up as:  {x:[int, [node with constant 3, node with constant 4], [temp for var 1, temp for var 2],2, 0]}
        self.array_dict = {}

        #handled_returns is a list of return nodes that have already been handled - used to check if we've handled a return or not
        self.handled_returns = []

        #Dictionary of all structs and the things within them: if a struct within a struct, both will show up as keys
        #Format is: {struct_name: [declaration1, declaration2, ...]}
        self.struct_dict = {}

        #Dictionary of all of the typedefs that point to types, including structs
        #Format is: {typedef_name: actual_type_name}
        self.typedef_dict = {}

    '''
    Replace preprocessor directives with empty lines to allow pycparser to parse the file correctly
    '''
    def remove_preprocessor_directives(self, user_script):
        lines = user_script.split('\n')
        new_lines = [self.clear_directive_line(l) for l in lines]
        new_user_script = '\n'.join(new_lines)

        return new_user_script


    def clear_directive_line(self, line):
        if ('_Generic' in line) or ('#include' in line):
            self.removed_lines.append(line)
            return '\r'
        else:
            return line

    """Finds all function declarations in the AST and puts them into our function list"""
    def find_all_function_decl(self, ast):
        i = 0
        while i < len(ast.ext):
            if isinstance(ast.ext[i], c_ast.FuncDef):
                if isinstance(ast.ext[i].decl.type.type, c_ast.PtrDecl):
                    tname = ast.ext[i].decl.type.type.type.type.names[0] + " *"
                else:
                    tname = ast.ext[i].decl.type.type.type.names[0]
                var_dict_add = {(str)(ast.ext[i].decl.name):tname}
                self.func_list.update(var_dict_add)
            i+=1


    """Splits up the AST by function, continues to recurse if a node has a compound node"""
    def recurse_by_function(self, ast):
        i = 0
        while i < len(ast.ext):
            func_name = None
            if isinstance(ast.ext[i], c_ast.FuncDef):
                func_name = ast.ext[i].decl.name

                #Add the list of global print nodes to the beginning of the main function before we continue
                if func_name == "main":
                    for node_num in range(0, len(self.global_print_nodes)):
                        (ast.ext)[i].body.block_items.insert(node_num, self.global_print_nodes[node_num])

            self.recurse_by_compound(ast.ext, i, func_name)
            i+= 1


    """Recurses through the AST by compound, which is the body of any node that has more nodes inside it"""
    def recurse_by_compound(self, parent, index, func_name):

        global to_add_index

        #If node is a node we added, ignore it & return 
        try:
            if parent[index].coord == None:
                return
        except:
            return

        ast_function = parent[index]

        self.handle_nodetypes(parent, index, func_name)
        try:
            compound_list = ast_function.body.block_items
        except AttributeError:
            try:
                compound_list = ast_function.stmt.block_items
            except AttributeError:
                try:
                    if_compound_list = [ast_function.iftrue.block_items]
                    compound_list = []
                    #Append any "if false" part of the if statement if exists
                    try:
                        if_compound_list.append(ast_function.iffalse.block_items)
                    #Case where there's another "if" inside this one
                    except AttributeError:
                        try:
                            if_compound_list.append([ast_function.iffalse])
                        except:
                            pass
                except:
                    return

        #Case for if statements: need to do both left and right side of "if"
        if len(compound_list) == 0 and len(if_compound_list) > 0:
            for each_compound in if_compound_list:
                compound_list = each_compound
                cur_par_index = 0
                while cur_par_index < len(compound_list):
                    self.recurse_by_compound(compound_list, cur_par_index, func_name)
                    cur_par_index += 1+to_add_index
                    to_add_index = 0
            if_compound_list = []

        #Case for non if statements
        else:
            cur_par_index = 0
            while cur_par_index < len(compound_list):
                self.recurse_by_compound(compound_list, cur_par_index, func_name)
                cur_par_index += 1+to_add_index
                to_add_index = 0



    """The initial function that's called: calls all the necessary functions above"""
    def add_printf(self, user_script):

        stripped_user_script = self.remove_preprocessor_directives(user_script)
        #print("STRIPPED USER SCRIPT IS -------")
        #print(stripped_user_script)

        #Need to save user_script in a temp file so that we can run it
        temp_c_file = self.temp_path + self.user + self.date_time + ".c"
        print("TEMP PATH IS -------"+(str)(self.temp_path))
        try:
            # Creating the C file, and create the temp directory if it doesn't exist
            try:
                f = open(temp_c_file, 'w')
            except OSError:
                # Create temp directory if it doesn't exist
                os.makedirs(os.path.dirname(self.temp_path))
                f = open(temp_c_file, 'w')

            f.write(stripped_user_script)
            f.close()

        except Exception as e:
            print("ERROR with user file pre-processing: {0}".format(e))
            return

        ast = parse_file(temp_c_file, use_cpp=True,
        cpp_path='gcc',
        cpp_args=['-nostdinc','-E', r'-Iutils/fake_libc_include'])

        try:
            os.remove(temp_c_file)
        except OSError:
            pass

        #Finding all functions in the program so we can save them in a list
        self.find_all_function_decl(ast)

        #Going through each function and adding all the print statements
        self.recurse_by_function(ast)

        #Turning the new ast back into C code
        generator = c_generator.CGenerator()

        return generator.visit(ast)


    """Takes a node, checks its type, and calls the appropriate function on it to add a print statement"""
    def handle_nodetypes(self, parent, index, func_name):
        current_location = LocationInfo(parent, index, func_name)

        self.base_node_check(parent[index], current_location)

        to_add_index = current_location.added_before

    """Base check of the node: calls different functions based on the instance of the node"""
    def base_node_check(self, node, location_node):
        if isinstance(node, c_ast.Decl):
            self.declaration_check(node, location_node)

    """If we have a declaration node, check if it's a typedecl or a pointerdecl"""
    def declaration_check(self, node, location_node):
        declaration_type = node.children()[0][1]

        if isinstance(declaration_type, c_ast.TypeDecl):
            self.type_declaration_check(node, location_node)
        
        elif isinstance(declaration_type, c_ast.PtrDecl):
            self.pointer_declaration_check(node, location_node)

    """If we have a TypeDeclaration node, check if it's a struct or a regular type declaration, like int"""
    def type_declaration_check(self, node, location_node):
        #This is the subtype of the type declaration node, such as "int" or "char" 
        typedecl_type = self.get_typedecl_type(node)

        #self.actual_type is the actual type of the declaration: ie, if its typedef'd 
        actual_type = self.get_actual_type(typedecl_type)
        
        is_struct = self.check_struct(actual_type)

        if is_struct:
            self.handle_struct_typedecl(self, node, location_node)

        else:
            self.handle_regular_typedecl(node, location_node)
            
    """Begin Typedecl Helper Functions"""

    #gets the typedecl type with a try/catch exception, which depends on what's inside our decl node    
    def get_typedecl_type(self, node):
        try:
            #This will pass as long as node_to_consider.type.type is an IdentifierType node
            return node.type.type.names[0] 
        except:
            #This will generally happen only when it is a struct that has already been declared, inside another struct
            return node.type.type.name
    
    #Returns the actual type of the TypeDeclaration, if not typedef'd it stays the same
    def get_actual_type(self, typedecl_type):
        typedef_entry = self.typedef_dict.get(typedecl_type)
        if typedef_entry != None:
            return typedef_entry
        else:
            return typedecl_type

    #Check if its actual_type is inside the struct dictionary: if so, return True
    def check_struct(self, actual_type):
        struct_entry = self.struct_dict.get(actual_type)   
 
        if struct_entry != None:
            return True
        else:
            return False

    def get_funccall_funcname(self, node):
        return node.name.name

    """End Typedecl Helper Functions"""

    """Handle adding nodes for a struct typedeclaration node"""
    def handle_struct_typedecl(self, node, location_node):
        print("struct typedecl")

    """Handle adding nodes for a regular type declaration node"""
    def handle_regular_typedecl(self, node, location_node):
        #Create a new regular typedeclaration object
        node_object = RegularTypeDeclaration(node, self, location_node)

        #If it's being assigned to an inside function, handle it slightly differently
        if node_object.is_funcdecl:
            self.handle_inside_funccall(node_object, location_node)

        #Otherwise, just add a single printf node right after this node in the AST
        else:
            self.add_one_printf(node_object, location_node)

    """If we called a function that we defined in our program, handle adding print nodes for it here"""
    def handle_inside_funccall(self, node_object, location_node):
        print("handling inside funccall")

    """This function handles adding a single print node 1+additional_space after our current index"""
    def add_one_printf(self, node_object, location_node):
        printf_object = PrintNode(node_object)

        #If the declaration happened outside a function, must be global
        if location_node.func_name == None:
            self.global_print_nodes.append(printf_object.print_node)

        else:
            place_to_add = location_node.added_before + location_node.initial_index + location_node.additional_space + 1
            location_node.parent.insert(place_to_add, printf_object.print_node)
            location_node.added_after += 1

