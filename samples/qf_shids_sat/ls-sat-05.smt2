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


(declare-const S_p SetInt)
(declare-const S_pp SetInt)
(declare-const S SetInt)
(declare-const S_t SetInt)

(declare-const root1 RefSls_t)
(declare-const cur RefSls_t)
(declare-const cur_p RefSls_t)

(declare-const key Int)

(assert 
		(and 
			(distinct cur nil)
			(distinct cur_p nil)
            (<= (min S_p) (- key 1))
            (= S_p (setunion S_pp (set (min S_p))))
            (= S_t (setminus S S_p))
            (=> (distinct S_t emptyset) (<= (max S_t) (- key 1)))

		(sep 
			(sls root1 S cur S_p)
            (pto cur (c_Sls_t cur_p (min S_p)))
            (sls cur_p S_pp nil emptyset)
		)
		)

)

(check-sat)

; unsat