(set-logic QF_SLID_SET)

(declare-sort RefSls_t 0)

(declare-datatypes (
	(Sls_t 0)
	) (
	((c_Sls_t (next RefSls_t)  ))
	)
)


(declare-heap (RefSls_t Sls_t) 
)

(define-fun-rec lls ((?E RefSls_t) (?S SetInt) (?F RefSls_t) (?S1 SetInt)) Bool
	(or 
		(and 
			(= ?E ?F)
			(= ?S ?S1)
		)

		(exists ((?X RefSls_t) (?S2 SetInt))
	 
		(and 
			(= ?S (setunion ?S2 (set (max ?S))))
			(= (max ?S2) (- (max ?S) 1))
		(sep 
			(pto ?E (c_Sls_t ?X ))
			(lls ?X ?S2 ?F ?S1)
		)

		)

		)

	)
)


(declare-const S1 SetInt)
(declare-const S2 SetInt)
(declare-const S3 SetInt)

(declare-const E1 RefSls_t)
(declare-const E2 RefSls_t)
(declare-const E3 RefSls_t)

(declare-const key Int)

(assert 
		(and 
			(<= (min S1) (max S1))
			(= (max S1) (+ (max S3) 1))
		(sep 
            (pto E1 (c_Sls_t E2  ))
            (pto E2 (c_Sls_t E1 ))
            (lls E1 S1 E3 S3)
		)
		)
)

(check-sat)

; unsat

