(logic QF_SLAH

 :smt-lib-version 2.6
 :written-by "Su Wanyun"
 :date "2021-04-08"
 :last-updated "2021-04-08"

 :theories (Ints SepLogicTyped)

 :language
 "Closed formulas built over the Separation Logic with Indunctive Definition and Set Data Constraints,
  with the following restrictions:
  - formulas are only conjunction of pure or spatial atoms, i.e., 
    they belong to the symbolic heap fragment,
  - 'wand' is not used, and
  - the inductively defined predicates are `well formed` like in SMT-LIB standard.
 "

 :extensions 
 "Possible extensions are:
 
  - boolean combination of atoms,
  
  - set data,
  "
)

