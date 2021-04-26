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
			(= ?S (setunion ?S2 (set (max ?S))))
			(= (max ?S) (+ (max ?S2) 1))
		(sep 
			(pto ?E (c_Sls_t ?X (max ?S)))
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
(declare-const F1 RefSls_t)

(assert 
		(and 
			(distinct E F)
			(distinct E1 F1)
			(>= (max S1) (+ (max S2) 1))
			(= (min S1) (+ (max S3) 1))

			(>= (max S3) (+ (max S4) 2))
			(= (max S2) (+ (min S2) 1))
		(sep 
			(sls E S1 F S2)
			(sls E1 S3 F1 S4)
		)
		)

)

(check-sat)
