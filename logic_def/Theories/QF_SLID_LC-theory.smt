



(theory QF_SLID_LC

 :smt-lib-version 2.0
 :written_by "Gao Chong"
 :date "2017-10-1"
 :last_modified "2018-6-11"

 :sorts ((Field 2)  (SetRef 1) (Space 0) (Rat 0) (Int 0) )

 :funs  ((emp Space)
         (ssep Space Space :left-assoc)
         (par (A) (pto A (SetRef A) Space :left-assoc))
         (tobool Space Bool)
         (tospace Bool Space)
         (par (A B) (ref (Field A B) B (SetRef A)))
         (par (A) (sref (SetRef A) (SetRef A) (SetRef A) :left-assoc))

         )

 :notes "The generic -- program independent -- signature for the QF_SLID_LC logic."

 :definition
 "The QF_SLID_LC theory corresponds to signature of the QF_SLID_LC logic
  in which:
  - the sort Field denotes the set of reference fields defined in the program;
    a reference field is typed by two sorts, thus its arity is 2;
  - the sort SetRef denotes the set of typed location variables;
  - the sort Space denotes the set of spatial formulas;
  - the sort Rat denotes the set of rational formulas;
  - the sort Int denotes the set of integer formulas;

  - for all sp in Space, v is a location variable, sr in SetRef, 
             f in Field:
      - emp denotes the empty heap space constraint;

      - (ssep sp sp) denotes the strong separating space constraint;

      - (pto v sr) denotes the points-to space constraint from location v;

      - (toBool sp) transforms a space constraint into a boolean constraint;

      - (toSpace b) transforms a boolean formula into a space formula,
        mainly used to be able to type quantifiers in space formulas;

      - (ref f v) denotes the tuple used in a points-to constraint, where
        f is a reference field which value is v, type is a set;

      - (sref sr sr) denotes the union of sets of tuples used in the points-to
        constraint;

 "

)
