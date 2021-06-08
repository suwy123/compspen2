(set-logic QF_SLRDI)

;(declare-sort Sls_t 0)
;(declare-fun next() (Field Sls_t Sls_t))
;(declare-fun data() (Field Sls_t Int))

; Sorts for locations, one by cell sort
(declare-sort Sls_t 0)

; Types of cells in the heap

(declare-datatypes (
	(Sls 0)
	) (
	((c_Sls_t (next Sls_t) ))
	)
)

(define-fun sls ((?E Sls_t) (?S SetInt) (?F Sls_t) (?S1 SetInt)) Space
    (tospace
        (or
            (and (= ?E ?F) (= ?S ?S1))

            (exists ((?X Sls_t) (?S2 SetInt))
                    (and
                        ; (= ?S ?S2)
                        (= ?S (setunion ?S2 (set (min ?S))))
                        (= (min ?S2) (+ (min ?S) 1))
                        ; (<= (max ?S) (+ (min ?S) ))
                        ; (= ?S (setunion ?S2 (set (min ?S))))
                        ;(= (max ?S) (+ (max ?S2) 3))
                        ; (<= (min ?S2) (- (min ?S) 2))
                        ; (> (min ?S2) (min ?S))
                        (tobool (ssep
                                 (pto ?E (sref (ref next ?X) (ref data min(?S))))
                                 (sls ?X ?S2 ?F ?S1)
                         ))
                    )
            )
        )
    )
)


(declare-fun S1() SetInt)
(declare-fun S2() SetInt)
(declare-fun S3() SetInt)
(declare-fun S4() SetInt)
(declare-fun S() SetInt)


(declare-fun E() Sls_t)
(declare-fun F() Sls_t)
(declare-fun E1() Sls_t)
(declare-fun F1() Sls_t)


(assert (and
        (>= (min S2)  (+ (min S1) 1))
        (= (min S1) (+ (max S3) 1))
        ; (= (- (min S1) (max S3)) 1)
        ; (= (max S1) (+ (min S1) 1))

        (<= (min S4)  (+ (min S3) 2))
        (= (- (max S2) (min S2)) 1)
        ; (= (max S4) (+ (min S4) 1))
        
        (tobool
           (ssep (sls E S1 F S2) (sls E1  S3 F1  S4))
           ; (sls E S1 F S2)
        )
)

)

(check-sat)
