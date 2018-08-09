/**
 * \file option.h
 * 
 * \author Adam Marshall (ih8celery)
 *
 */
#ifndef _MOD_CPP_COMMAND_PARSE_OPTION

#define _MOD_CPP_COMMAND_PARSE_OPTION

namespace cli {
  namespace Property {
    /**
     * \enum Number
     * \brief defines the number of times an option may appear
     *
     * <number> := <nil> // Number::ZERO_ONE <br>
     *          | '?'    // Number::ZERO_ONE <br>
     *          | '*'    // Number::ZERO_MANY <br>
     */
    enum class Number {
      ZERO_ONE, ZERO_MANY
    };

    /**
     * \enum Assignment
     * \brief defines the mode of assignment supported by option
     *
     * <assignment> := <nil> // Assignment::NONE <br>
     *              | '='   // Assignment::EQ_REQUIRED <br>
     *              | '=?'  // Assignment::EQ_MAYBE <br>
     *              | '=!'  // Assignment::EQ_NEVER <br>
     *              | '=|'  // Assignment::STUCK_ARG <br>
     */
    enum class Assignment {
      NO_ASSIGN, EQ_REQUIRED, EQ_MAYBE, EQ_NEVER, STUCK
    };
    
    /**
     * \enum Collection
     * \brief defines how arguments are interpreted
     *
     * <collection> := <nil>            // Collection::SCALAR <br>
     *              | '['<data_prop>']' // Collection::LIST <br>
     */
    enum class Collection {
      SCALAR, LIST
    };
    
    /**
     * \enum Arg_Type
     * \brief defines the type of data of an option's argument
     *
     * <data_type> := <nil> // Arg_Type::STRING <br>
     *             | 's'   // Arg_Type::STRING <br>
     *             | 'i'   // Arg_Type::INTEGER <br>
     *             | 'f'   // Arg_Type::FLOAT <br>
     */
    enum class Arg_Type {
      STRING, INTEGER, FLOAT
    };
  }

  /**
   * \struct Option
   * \brief collect the properties and identity of option
   */
  class Option {
    public:
      Option();

      Property::Number number;
      Property::Assignment assignment;
      Property::Collection collection;
      Property::Arg_Type type;
      std::string name;

      friend bool operator<(const Option&, const Option&) noexcept;
      friend bool operator==(const Option&, const Option&) noexcept;
  };
}

#endif
