(set-logic QF_SLID_INT)

(declare-sort Refnode 0)

(declare-datatypes
 ((node 0))
 (((c_node (next Refnode) (prev Refnode)))))

(declare-heap (Refnode node))

;; declare predicates
(define-fun-rec ldll ((?E Refnode) (?P Refnode) (?len1 Int) (?F Refnode) (?L Refnode) (?len2 Int)) Bool 
	(or 
	(and (= ?E ?F) (= ?P ?L) (= ?len1 ?len2)
		(_ emp Refnode node)
	)
 
	(exists ((?u Refnode) (?len3 Int)) 
		(and 
			(= ?len1 (+ ?len3 1)) 
			(sep 
				(pto ?E (c_node ?u ?P ) ) 
				(ldll ?u ?E ?len3 ?F ?L ?len2)
			)
		)
	)

	)
)

;(define-fun dll ((hd_1 Refnode) (p_2 Refnode) (tl_3 Refnode) (n_4 Refnode) (l_5 Int)) Space 
;(tospace  (or
;          (and (= (+ l_5 (- 1)) 0)
;               (= hd_1 tl_3)
;               (pto hd_1 (c_node n_4 p_2))
;          )
;          (exists ((x_6 Refnode) (k Int))
;              (and (= k (- l_5 1))
;                   (<= 1 (- l_5 1))
;                   (ssep
;                       (pto hd_1 (c_node x_6 p_2))
;                       (dll x_6 hd_1 tl_3 n_4 k))
;))))
;;;;dll(hd,p,tl,n,l) = ldll(hd,p,l;n,tl,0)

;(define-fun-rec dllnull ((hd_7 Refnode) (p_8 Refnode) (l_9 Int)) Bool
;  (or
;   (and
;    (pto hd_7 (c_node (as nil Refnode) p_8))
;    (= (- l_9 1) 0))
;   (exists
;    ((x_10 Refnode) (k Int))
;    (and
;     (sep
;      (pto hd_7 (c_node x_10 p_8))
;      (dllnull x_10 hd_7 k))
;     (= k (- l_9 1))
;     (<= 1 (- l_9 1))))))
;;;;dllnull(hd,p,l) = \exists tl.ldll(hd,p,l,nil,tl,0)


;; entailment: dll(x,y,z,t,n) * dllnull(t,z,m) |- dllnull(x,y,m+n)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const z Refnode)
(declare-const t Refnode)
(declare-const n Int)
(declare-const m Int)
(declare-const k Int)
(declare-const tl Refnode)

;(assert
; (and
;  (= k (+ m n))
; (tobool(ssep
;  (ldll x y n t z 0)
;  (ldll t z m nil tl 0)
; ))
;))

(assert
 (and
  (= k (+ m n))
  (sep
   (ldll x y k t z m)
   (ldll t z m nil tl 0)
  )
))



(assert
 (not
  (ldll x y k nil tl 0)
))


(check-sat)
;;expect:unsat
