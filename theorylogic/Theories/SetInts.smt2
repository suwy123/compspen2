(theory SetInts 

 :smt-lib-version 2.0
 :written_by "Gao Chong"
 :date "2018-10-1"
 :last_modified "2019-2-18"

 :sorts ( (SetInt 0) )

 :funs  (
         (emptyset SetInt)
         (set Int SetInt)
         (setunion SetInt SetInt SetInt :left-assoc)
         (setintersect SetInt SetInt SetInt :left-assoc)
         (setminus SetInt SetInt SetInt)
         (subset SetInt SetInt Bool)
         (psubset SetInt SetInt Bool)
         (belongsto Int SetInt Bool)
         (min SetInt Int)
         (max SetInt Int)
         )

 :notes "The generic -- program independent -- signature for the SetInts logic."

 :definition
 "The SetInts theory corresponds to signature of the SetInts logic
  in which:

  - the sort Int denotes the set of integer formulas;
   - for all i in Int, S1, S2 in SetInt:
      - emptyset denotes a empty set;

      - (set i) denotes the singleton set of element i;

      - (setunion S1 S2) denotes set union;

      - (setintersect S1 S2) denotes set setintersect;

      - (setminus S1 S2) denotes set difference S1 \ S2;

      - (subset S1 S2) is true if all elements of S1 are in S2;

      - (psubset S1 S2) is true if all elements of S1 are in S2 and S1 != S2;

      - (belongsto i S1) is true if i in set S1;

      - (min S1) denotes min value of set S1;

      - (max S2) denotes max value of set S2;

 "

)
