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
			; (<= (min ?S) (max ?S) )
		(sep 
			(pto ?E (c_Dll_t ?u ?P  (min ?S)))
			(sdll ?u ?E ?S2 ?F ?L ?S1 )
		)

		)

		)

	)
)

;; variables
(declare-const S1 SetInt)
(declare-const S2 SetInt)
(declare-const S3 SetInt)
(declare-const S4 SetInt)
(declare-const S5 SetInt)

(declare-const E1 RefDll_t)
(declare-const E1_p RefDll_t)
(declare-const E2 RefDll_t)
(declare-const E2_p RefDll_t)
(declare-const E3 RefDll_t)
(declare-const E3_p RefDll_t)
(declare-const E4 RefDll_t)
(declare-const E4_p RefDll_t)
(declare-const E5 RefDll_t)
(declare-const E5_p RefDll_t)

(assert 
		(and 
			(distinct E1 E2)
		(sep 
            (sdll E1 E1_p S1 E2 E3 S2)
            (sdll E2 E3 S2 E4 E1 S4)
		)
		)
)



(check-sat)
