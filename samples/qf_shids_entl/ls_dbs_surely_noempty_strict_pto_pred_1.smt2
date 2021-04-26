(set-logic QF_SLID_SET)

(declare-sort RefSls_t 0)

(declare-datatypes (
	(Sls_t 0)
	) (
	((c_Sls_t (next RefSls_t) (data Int) ))
	)
)


(declare-heap (RefSls_t Sls_t) 
)

(define-fun-rec sls ((?E RefSls_t) (?S SetInt) (?F RefSls_t) (?S1 SetInt)) Bool
	(or 
		(and 
			(= ?E ?F)
			(= ?S ?S1)
		)

		(exists ((?X RefSls_t) (?S2 SetInt))
	 
		(and 
			(= ?S (setunion ?S2 (set (min ?S))))
			(= (min ?S2) (+ (min ?S) 1))
		(sep 
			(pto ?E (c_Sls_t ?X (min ?S)))
			(sls ?X ?S2 ?F ?S1)
		)

		)

		)

	)
)


(declare-const S1 SetInt)
(declare-const S2 SetInt)
(declare-const S3 SetInt)
(declare-const S4 SetInt)
(declare-const S SetInt)

(declare-const E RefSls_t)
(declare-const F RefSls_t)
(declare-const E1 RefSls_t)
(declare-const E2 RefSls_t)
(declare-const F1 RefSls_t)

(assert 
		(and 
			(= E2 F)
			(= S1 (setunion S2 (set (min S1))))
			(>= (min S2) (+ (min S1) 1))
		(sep 
			(pto E (c_Sls_t F (min S1)))
			(sls E2 S2 F1 S4)
		)
		)
)

(assert (not
	(and
		(distinct E F1)	
		(sep 
			(sls E S1 F1 S4)
		)
	)
))

(check-sat)
