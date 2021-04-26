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
(declare-const E1 RefDll_t)
(declare-const E2 RefDll_t)
(declare-const F1 RefDll_t)
(declare-const F2 RefDll_t)

(declare-const SS1 SetInt)
(declare-const SS2 SetInt)

(assert 
	
	(and 
			(= E1 F2)
			(<= (min SS1) (+ (min SS2) 3 ) )
			(pto F2 (c_Dll_t E1 F1 (min SS1) ))
			(sdll E1 E2 SS1 E1 E2 SS2 )
		)
)

(check-sat)
