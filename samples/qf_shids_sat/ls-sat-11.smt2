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
			(<= (min ?S2) (+ (min ?S) 1))
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
(declare-const S5 SetInt)

(declare-const E1 RefSls_t)
(declare-const E2 RefSls_t)
(declare-const E3 RefSls_t)
(declare-const E4 RefSls_t)
(declare-const E5 RefSls_t)

(assert 
		(and 
			(distinct E1 E3)
			(distinct E1 E4)
			(distinct S2 S3)
			(= (min S4) (min S1))
		(sep 
            (sls E1 S1 E3 S3)
            (sls E2 S2 E3 S3)
            (sls E1 S1 E4 S4)
            (sls E4 S4 E5 S5)
		)
		)
)

(check-sat)
; unsat
