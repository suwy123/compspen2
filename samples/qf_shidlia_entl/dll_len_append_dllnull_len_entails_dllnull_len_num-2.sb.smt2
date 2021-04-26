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
		(_ emp Refnode node
		)
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

;;;;dll = ldll(hd,p,l;n,tl,0)
;;;;dllnull = \exists tl.ldll(hd,p,l;nil,tl,0)

;; entailment: dll(x,y,z,t,100) * dllnull(t,z,m) |- dllnull(x,y,m+100)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const z Refnode)
(declare-const t Refnode)
(declare-const m Int)
(declare-const k100 Int)
(declare-const k Int)
(declare-const tl Refnode)

(assert
 (and (= k100 100) (= k (+ m 100))
  (sep
   (ldll x y k t z m)
   (ldll t z m nil tl 0))
))

(assert
 (not
  (ldll x y k nil tl 0)))

(check-sat)
;; unsat
