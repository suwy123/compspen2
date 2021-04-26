(set-logic QF_SLID_SET)

; Sorts for locations, one by cell sort
(declare-sort RefDll_t 0)

; Types of cells in the heap

(declare-datatypes (
	(Dll_t 0)
	) (
	((c_Dll_t (next RefDll_t) (prev RefDll_t) (data Int) ))
	)
)

; Type of heap

(declare-heap (RefDll_t Dll_t) 
)
; User defined predicate
(define-fun-rec sdll ((?E RefDll_t)(?P RefDll_t)(?S SetInt)(?F RefDll_t)(?L RefDll_t)(?S1 SetInt)) Bool
	(or 
		(and 
			(= ?E ?F)
			(= ?P ?L)
			(= ?S ?S1)
			(_ emp RefDll_t Dll_t)
		)

	(exists ((?u RefDll_t)(?S2 SetInt))
	 
		(and 
			(= ?S (setunion ?S2 (set (min ?S))))
			(= (min ?S2) (+ (min ?S) 1))
		(sep 
			(pto ?E (c_Dll_t ?u ?P  (min ?S)))
			(sdll ?u ?E ?S2 ?F ?L ?S1 )
		)

		)

		)

	)
)

;; variables
(declare-const root1 RefDll_t)
(declare-const root_p RefDll_t)
(declare-const cur RefDll_t)
(declare-const cur_p RefDll_t)
(declare-const cur_pp RefDll_t)
(declare-const tail_pp RefDll_t)



(declare-const S_p SetInt)
(declare-const S_pp SetInt)
(declare-const S SetInt)
(declare-const S_t SetInt)

(declare-const key Int)

(assert 
	(and 
			(distinct cur nil)
            (<= (min S_p) (- key 1))
            (= S_p (setunion S_pp (set (min S_p))))
            (= S_t (setminus S S_p))
            (=> (distinct S_t emptyset) (<= (max S_t) (- key 1)))

		(sep 
			(sdll root1 root_p S cur cur_pp S_p)
            (pto cur (c_Dll_t cur_p cur_pp  (min S_p)))
            (sdll cur_p cur S_pp nil nil emptyset)
		)
	)
)

(check-sat)
