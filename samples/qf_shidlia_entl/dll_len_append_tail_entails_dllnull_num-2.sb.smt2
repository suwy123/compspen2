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
))

;;;;dll = ldll(hd,p,l;n,tl,0)
;;;;dllnull = \exists tl.ldll(hd,p,l,nil,tl,0)

;; entailment: t->node{nil,z} * dll(x,y,z,t,n) & 100<=n |- dllnull(x,y,n+1)

(declare-const t Refnode)
(declare-const z Refnode)
(declare-const x Refnode)
(declare-const y Refnode)
(declare-const n Int)
(declare-const k Int)

(assert
 (and (= k (+ n 1)) (<= 100 n)
  (sep
   (pto t (c_node  nil z))
   (ldll x y k t z 1))
))

(assert
 (not
  (ldll x y k nil t 0)
))

(check-sat)
;;unsat
