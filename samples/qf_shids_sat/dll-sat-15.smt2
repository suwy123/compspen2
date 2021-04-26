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
			(= ?S (setunion ?S2 (set (max ?S))))
			(= (max ?S) (+ (max ?S2) 1))
		(sep 
			(pto ?E (c_Dll_t ?u ?P  (min ?S)))
			(sdll ?u ?E ?S2 ?F ?L ?S1 )
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

(declare-const E RefDll_t)
(declare-const E_p RefDll_t)
(declare-const F RefDll_t)
(declare-const F_p RefDll_t)
(declare-const F1 RefDll_t)
(declare-const F1_p RefDll_t)

(assert 
	(and 
		(distinct E F)
		(= S2 emptyset)

		(sep 
			(sdll E E_p S1 F F_p S2)
			(pto F (c_Dll_t F1 F_p (min S3) ))
		)
	)

)

(check-sat)

