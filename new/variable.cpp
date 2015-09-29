#include "variable.h"

Data::Data (std::string _name, Type::TypeID _type_id, Mod _modifier, bool _is_static) {
    type = Type::init (_type_id);
    name = _name;
    modifier = _modifier;
    is_static = _is_static;
    class_id = MAX_CLASS_ID;
    align = 0;
}

Variable::Variable (std::string _name, Type::TypeID _type_id, Mod _modifier, bool _is_static) : Data (_name, _type_id, _modifier, _is_static) {
    class_id = Data::VarClassID::VAR;
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            value.bool_val = type->get_min ();
            min.bool_val = type->get_min ();
            max.bool_val = type->get_max ();
            break;
        case Type::TypeID::CHAR:
            value.char_val = type->get_min ();
            min.char_val = type->get_min ();
            max.char_val = type->get_max ();
            break;
        case Type::TypeID::UCHAR:
            value.uchar_val = type->get_min ();
            min.uchar_val = type->get_min ();
            max.uchar_val = type->get_max ();
            break;
        case Type::TypeID::SHRT:
            value.shrt_val = type->get_min ();
            min.shrt_val = type->get_min ();
            max.shrt_val = type->get_max ();
            break;
        case Type::TypeID::USHRT:
            value.ushrt_val = type->get_min ();
            min.ushrt_val = type->get_min ();
            max.ushrt_val = type->get_max ();
            break;
        case Type::TypeID::INT:
            value.int_val = type->get_min ();
            min.int_val = type->get_min ();
            max.int_val = type->get_max ();
            break;
        case Type::TypeID::UINT:
            value.uint_val = type->get_min ();
            min.uint_val = type->get_min ();
            max.uint_val = type->get_max ();
            break;
        case Type::TypeID::LINT:
            value.lint_val = type->get_min ();
            min.lint_val = type->get_min ();
            max.lint_val = type->get_max ();
            break;
        case Type::TypeID::ULINT:
            value.ulint_val = type->get_min ();
            min.ulint_val = type->get_min ();
            max.ulint_val = type->get_max ();
            break;
        case Type::TypeID::LLINT:
            value.llint_val = type->get_min ();
            min.llint_val = type->get_min ();
            max.llint_val = type->get_max ();
            break;
        case Type::TypeID::ULLINT:
            value.ullint_val = type->get_min ();
            min.ullint_val = type->get_min ();
            max.ullint_val = type->get_max ();
            break;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN VARIABLE" << std::endl;
            break;
    }
}

void Variable::dbg_dump () {
    std::cout << "type ";
    std::cout << std::endl;
    std::cout << "name: " << name << std::endl;
    std::cout << "modifier: " << modifier << std::endl;
    std::cout << "value: " << value.uint_val << std::endl;
    std::cout << "min: " << min.uint_val << std::endl;
    std::cout << "max: " << max.uint_val << std::endl;
}

void Data::set_value (uint64_t _val) {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            value.bool_val = (bool) _val;
            break;
        case Type::TypeID::CHAR:
            value.char_val = (signed char) _val;
            break;
        case Type::TypeID::UCHAR:
            value.uchar_val = (unsigned char) _val;
            break;
        case Type::TypeID::SHRT:
            value.shrt_val = (short) _val;
            break;
        case Type::TypeID::USHRT:
            value.ushrt_val = (unsigned short) _val;
            break;
        case Type::TypeID::INT:
            value.int_val = (int) _val;
            break;
        case Type::TypeID::UINT:
            value.uint_val = (unsigned int) _val;
            break;
        case Type::TypeID::LINT:
            value.lint_val = (long int) _val;
            break;
        case Type::TypeID::ULINT:
            value.ulint_val = (unsigned long int) _val;
            break;
        case Type::TypeID::LLINT:
            value.llint_val = (long long int) _val;
            break;
        case Type::TypeID::ULLINT:
            value.ullint_val = (unsigned long long int) _val;
            break;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN DATA: set_value" << std::endl;
            break;
    }
}

uint64_t Data::get_value () {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            return (bool) value.bool_val;
        case Type::TypeID::CHAR:
            return (signed char) value.char_val;
        case Type::TypeID::UCHAR:
            return (unsigned char) value.uchar_val;
        case Type::TypeID::SHRT:
            return (short) value.shrt_val;
        case Type::TypeID::USHRT:
            return (unsigned short) value.ushrt_val;
        case Type::TypeID::INT:
            return (int) value.int_val;
        case Type::TypeID::UINT:
            return (unsigned int) value.uint_val;
        case Type::TypeID::LINT:
            return (long int) value.lint_val;
        case Type::TypeID::ULINT:
            return (unsigned long int) value.ulint_val;
        case Type::TypeID::LLINT:
            return (long long int) value.llint_val;
        case Type::TypeID::ULLINT:
            return (unsigned long long int) value.ullint_val;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN DATA: get_value" << std::endl;
            return 0;
    }
}

void Data::set_max (uint64_t _max) {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            max.bool_val = (bool) _max;
            break;
        case Type::TypeID::CHAR:
            max.char_val = (signed char) _max;
            break;
        case Type::TypeID::UCHAR:
            max.uchar_val = (unsigned char) _max;
            break;
        case Type::TypeID::SHRT:
            max.shrt_val = (short) _max;
            break;
        case Type::TypeID::USHRT:
            max.ushrt_val = (unsigned short) _max;
            break;
        case Type::TypeID::INT:
            max.int_val = (int) _max;
            break;
        case Type::TypeID::UINT:
            max.uint_val = (unsigned int) _max;
            break;
        case Type::TypeID::LINT:
            max.lint_val = (long int) _max;
            break;
        case Type::TypeID::ULINT:
            max.ulint_val = (unsigned long int) _max;
            break;
        case Type::TypeID::LLINT:
            max.llint_val = (long long int) _max;
            break;
        case Type::TypeID::ULLINT:
            max.ullint_val = (unsigned long long int) _max;
            break;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN Data: set_max" << std::endl;
            break;
    }
}

uint64_t Data::get_max () {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            return (bool) max.bool_val;
        case Type::TypeID::CHAR:
            return (signed char) max.char_val;
        case Type::TypeID::UCHAR:
            return (unsigned char) max.uchar_val;
        case Type::TypeID::SHRT:
            return (short) max.shrt_val;
        case Type::TypeID::USHRT:
            return (unsigned short) max.ushrt_val;
        case Type::TypeID::INT:
            return (int) max.int_val;
        case Type::TypeID::UINT:
            return (unsigned int) max.uint_val;
        case Type::TypeID::LINT:
            return (long int) max.lint_val;
        case Type::TypeID::ULINT:
            return (unsigned long int) max.ulint_val;
        case Type::TypeID::LLINT:
            return (long long int) max.llint_val;
        case Type::TypeID::ULLINT:
            return (unsigned long long int) max.ullint_val;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN DATA: get_max" << std::endl;
            return 0;
    }
}

void Data::set_min (uint64_t _min) {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            min.bool_val = (bool) _min;
            break;
        case Type::TypeID::CHAR:
            min.char_val = (signed char) _min;
            break;
        case Type::TypeID::UCHAR:
            min.uchar_val = (unsigned char) _min;
            break;
        case Type::TypeID::SHRT:
            min.shrt_val = (short) _min;
            break;
        case Type::TypeID::USHRT:
            min.ushrt_val = (unsigned short) _min;
            break;
        case Type::TypeID::INT:
            min.int_val = (int) _min;
            break;
        case Type::TypeID::UINT:
            min.uint_val = (unsigned int) _min;
            break;
        case Type::TypeID::LINT:
            min.lint_val = (long int) _min;
            break;
        case Type::TypeID::ULINT:
            min.ulint_val = (unsigned long int) _min;
            break;
        case Type::TypeID::LLINT:
            min.llint_val = (long long int) _min;
            break;
        case Type::TypeID::ULLINT:
            min.ullint_val = (unsigned long long int) _min;
            break;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN Data: set_min" << std::endl;
            break;
    }
}

uint64_t Data::get_min () {
    switch (type->get_id ()) {
        case Type::TypeID::BOOL:
            return (bool) min.bool_val;
        case Type::TypeID::CHAR:
            return (signed char) min.char_val;
        case Type::TypeID::UCHAR:
            return (unsigned char) min.uchar_val;
        case Type::TypeID::SHRT:
            return (short) min.shrt_val;
        case Type::TypeID::USHRT:
            return (unsigned short) min.ushrt_val;
        case Type::TypeID::INT:
            return (int) min.int_val;
        case Type::TypeID::UINT:
            return (unsigned int) min.uint_val;
        case Type::TypeID::LINT:
            return (long int) min.lint_val;
        case Type::TypeID::ULINT:
            return (unsigned long int) min.ulint_val;
        case Type::TypeID::LLINT:
            return (long long int) min.llint_val;
        case Type::TypeID::ULLINT:
            return (unsigned long long int) min.ullint_val;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN DATA: get_min" << std::endl;
            return 0;
    }
}

Array::Array (std::string _name, Type::TypeID _base_type_id, Mod _modifier, bool _is_static,
              uint64_t _size, Ess _essence) : Data (_name, Type::TypeID::PTR, _modifier, _is_static) {
    class_id = Data::VarClassID::ARR;
    base_type = Type::init (_base_type_id);
    size = _size;
    essence = _essence;
    switch (base_type->get_id ()) {
        case Type::TypeID::BOOL:
            value.bool_val = type->get_min ();
            min.bool_val = type->get_min ();
            max.bool_val = type->get_max ();
            break;
        case Type::TypeID::CHAR:
            value.char_val = type->get_min ();
            min.char_val = type->get_min ();
            max.char_val = type->get_max ();
            break;
        case Type::TypeID::UCHAR:
            value.uchar_val = type->get_min ();
            min.uchar_val = type->get_min ();
            max.uchar_val = type->get_max ();
            break;
        case Type::TypeID::SHRT:
            value.shrt_val = type->get_min ();
            min.shrt_val = type->get_min ();
            max.shrt_val = type->get_max ();
            break;
        case Type::TypeID::USHRT:
            value.ushrt_val = type->get_min ();
            min.ushrt_val = type->get_min ();
            max.ushrt_val = type->get_max ();
            break;
        case Type::TypeID::INT:
            value.int_val = type->get_min ();
            min.int_val = type->get_min ();
            max.int_val = type->get_max ();
            break;
        case Type::TypeID::UINT:
            value.uint_val = type->get_min ();
            min.uint_val = type->get_min ();
            max.uint_val = type->get_max ();
            break;
        case Type::TypeID::LINT:
            value.lint_val = type->get_min ();
            min.lint_val = type->get_min ();
            max.lint_val = type->get_max ();
            break;
        case Type::TypeID::ULINT:
            value.ulint_val = type->get_min ();
            min.ulint_val = type->get_min ();
            max.ulint_val = type->get_max ();
            break;
        case Type::TypeID::LLINT:
            value.llint_val = type->get_min ();
            min.llint_val = type->get_min ();
            max.llint_val = type->get_max ();
            break;
        case Type::TypeID::ULLINT:
            value.ullint_val = type->get_min ();
            min.ullint_val = type->get_min ();
            max.ullint_val = type->get_max ();
            break;
        case Type::TypeID::PTR:
        case Type::TypeID::MAX_INT_ID:
        case Type::TypeID::MAX_TYPE_ID:
            std::cerr << "BAD TYPE IN VARIABLE" << std::endl;
            break;
    }
}

void Array::dbg_dump () {
    std::cout << "type ";
    type->dbg_dump ();
    std::cout << "base_type ";
    base_type->dbg_dump ();
    std::cout << "name: " << name << std::endl;
    std::cout << "size: " << size << std::endl;
    std::cout << "essence: " << essence << std::endl;
    std::cout << "modifier: " << modifier << std::endl;
    std::cout << "value: " << value.uint_val << std::endl;
    std::cout << "min: " << min.uint_val << std::endl;
    std::cout << "max: " << max.uint_val << std::endl;
}
