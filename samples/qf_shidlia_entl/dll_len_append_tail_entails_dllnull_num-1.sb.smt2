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

;;;;dll(hd,p,n,tl,l) = ldll(hd,p,l;n,tl,0)
;;;;dllnull(hd,p,l) = \exists tl.ldll(hd,p,l,nil,tl,0)

;; entailment: t->node{nil,z} * dll(x,y,z,t,100) |- dllnull(x,y,101)

(declare-const t Refnode)
(declare-const z Refnode)
(declare-const x Refnode)
(declare-const y Refnode)
(declare-const k100 Int)
(declare-const k101 Int)

(assert
 (and (= k100 100) (= k101 101)
 (sep
  (pto t (c_node nil z))
  (ldll x y k101 t z 1)))
))

(assert
 (not
  (ldll x y k101 nil t 0)
))

(check-sat)
;;unsat
