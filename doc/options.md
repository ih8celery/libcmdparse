# Creating Options

option\_t objects and their associated internal state are created by
invoking the `option` member function of the opt\_parser class, with
the following prototype:

`option_t option(const string& spec, const string& name = "")`

# Structure of Information Stored About Options

## Handles

a handle is a way of referring to options both internally and in ARGV.
handles are provided when the option is defined. commonly, handles
begin with "-" or "--". in the option spec, handles may be listed by
separating them with "|".

## Names

an option's name is its unique identifier. all definitions of options
with the same name must be compatible with each other. you can refer
to options by their name when defining them, as the second argument to
`option`, or in the public member functions of an opt\_info object.

## The option\_t struct

option\_t is a struct without private members that is used
by opt\_parser to store the information provided by `option`.

a simplified definition of the struct would look like this:

```c++
struct option_t {
  string name;
  Mod_Prop mod;
  Num_Prop number;
  Assign_Prop assignment;
  Collect_Prop collection;
  Data_Prop data_type;
}
```

each of the members shown except for name is described below.

### Mod\_Prop

describes modifier affecting an option

```c++
enum class Mod_Prop {
  NONE,
  SUB,
  BEFORE,
  AFTER,
  NOT_WITH
}
```

### Num\_Prop

specifies the number of times an option may appear in command line arguments

```c++
enum class Num_Prop {
  ZERO_ONE,
  ZERO_MANY
}
```

### Assign\_Prop

determines whether an option may take arguments and how its arguments
are arranged with respect to it's handles

```c++
enum class Assign_Prop {
  NO_ASSIGN,
  EQ_REQUIRED,
  EQ_MAYBE,
  EQ_NEVER,
  STUCK
}
```

### Collect\_Prop

an option's collection defines the manner in which arguments are
determined

```c++
enum class Collect_Prop {
  SCALAR,
  LIST
}
```

### Data\_Prop

defines the type of data of an option's argument(s)

```c++
enum class Data_Prop {
  STRING,
  INTEGER,
  FLOAT
}
```

## What Is an Option Spec?

the option spec is the first argument to `option`, which is a condensed
string representation of an option\_t object and the handles associated
with the option.

```
<option_spec> := <mod><handle_list><number><arg_spec>
<mod>         := '['<mod_expr>']'|<nil>
<mod_expr>    := <mod_setting>|<mod_fn><mod_arg>
<mod_setting> := '&'
<mod_fn>      := '<'|'>'|'!'
<mod_arg>     := <option_name>|'*'
<handle_list> := <handle>|<handle>'|'<handle_list>
<handle>      := <prefix><handle_name>
<prefix>      := '-'<minus>|'+'<plus>|'.'|':'|'/'|<nil>
<minus>       := '-'|<nil>
<plus>        := '+'|<nil>
<handle_name> := <option_name>
<number>      := '*'|'?'|<nil>
<arg_spec>    := <eq><arg_type>|<eq><arglist>|<arglist>|<nil>
<eq>          := '='<eq_type>
<eq_type>     := '?'|'!'|'|'|<nil>
<arg_type>    := 's'|'i'|'f'|<nil>
<arglist>     := '['<arg_type>']'
```

# Examples

the following example calls to `option` demonstrate how to create
common types of options. in the examples, assume `p` is an object
of type `opt_parser`.

## Set the Option Name Explicitly or Implicitly

an option's name can be set with the second argument to `option` (if
it is not the empty string). otherwise it is deduced from the last
handle in the handle list, minus the prefix.

`p.option("--handle", "name")`

the above option has the name "name".

`p.option("--handle")`

here the name is deduced to be "handle".

## Multiple Handles to an Option

you can define multiple handles which target the same option. this
can be achieved in a single option spec or across multiple specs.
note that in the latter case, the options deduced from the separate
specs must be compatible.

`p.option("-a|-b", "name")`

this call creates an option with name "name" and the handles "-a" and
"-b".

`p.option("-c", "name")`

this call adds a third handle "-c" to the same option, using the name
"name" to connect them.

## Multiple Types of Prefixes

libcmdparse supports several types of handle prefixes, including the
common "-", "--", and "+". you can define an option with handles
using every type of prefix or no prefix at all.

`p.option("-h|--h|.h|:h|+h|++h|/h|h")`

this example creates an option with handles using every prefix and also
no prefix.

## Simple Flags

`p.option("--has-option")`

this creates an option with no arguments that has the handle
"--has-option". implicitly, it is allowed to occur only once. you
may state this explicitly with

`p.option("--has-option?")`

or allow it to occur any number of times with

`p.option("--has-option*")`

## Options with Arguments

options may take string, integer, or floating point arguments,
depending on how they are declared. the default argument type is
string.

`p.option("-age=i")`

when the handle "-age" is found in parsing, optparser will enforce
three conditions:
1. an '=' must be used to separate the handle from its argument
2. the handle must have an argument
3. the argument must match an integer

an option that splits arguments on the comma may be specified as

`p.option("-W=|[s]")`

this option takes string arguments which are stuck to the handle,
like so

"-Wsome,any,none"

which receives the single argument "some,any,none" and splits it into
"some", "any", and "none".
