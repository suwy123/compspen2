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
(declare-const E1 RefSls_t)
(declare-const E2 RefSls_t)
(declare-const F RefSls_t)

(assert 
		(and 
			(>= (min S2) (+ (min S1) 1))
			(>= (min S3) (+ (min S1) 2))
			(>= (min S4) (+ (min S1) 3))
			(= S3 (setunion S4 (set (min S3))))
			(= S2 (setunion S3 (set (min S2))))
			(= S1 (setunion S2 (set (min S1))))
		(sep 
			(pto E (c_Sls_t E1 (min S1)))
			(pto E1 (c_Sls_t E2 (min S2)))
			(pto E2 (c_Sls_t F (min S3)))
		)
		)
)

(assert (not
	(and
		(distinct E F)	
		(sep 
			(sls E S1 F S4)
		)
	)
))

(check-sat)
