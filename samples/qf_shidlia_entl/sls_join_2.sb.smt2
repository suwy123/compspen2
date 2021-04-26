(set-logic QF_SLID_INT)

(declare-sort Refnode 0)

(declare-datatypes
 ((node 0))
 (((c_node (next Refnode) (data Int)))))

(declare-heap (Refnode node))

;; declare predicates
(define-fun-rec sls 
	((?in Refnode) (?dt1 Int)  (?out Refnode) (?dt2 Int)) 
	Bool
		(or 
			(and (= ?dt1 ?dt2) (= ?in ?out)
			(_ emp Refnode node)
			)
 
			(exists 
				((?u Refnode) (?dt3 Int)) 
				(and  	
					(< ?dt1 (+ ?dt3 1))
					(sep 
						(pto ?in (c_node ?u ?dt1)) 
						(sls ?u ?dt3 ?out ?dt2)
					) 
				)
			)
		)
)

;(define-fun-rec ls ((x_1 Refnode) (y_2 Refnode) (l_3 Int) (u_4 Int)) Bool
;  (or
;   (and
;    (pto x_1 (c_node y_2 l_3))
;    (= l_3 u_4))
;   (exists
;    ((t_5 Refnode) (a_6 Int))
;    (and
;     (sep
;      (pto x_1 (c_node t_5 l_3))
;      (ls t_5 y_2 a_6 u_4))
;     (and
;      (<= a_6 u_4)
;      (<= l_3 a_6))))))

;;;;ls(x,y,l,u) = \exists z. \wedge sls(x,l,z,u)*z|->(next:y,data:u)

;; entailment: ls(x,y,l1,u1) * ls(y,z,l2,u2) & u1<=l2 |- ls(x,z,l1,u2)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const l1 Int)
(declare-const u1 Int)
(declare-const z Refnode)
(declare-const l2 Int)
(declare-const u2 Int)

(declare-const z1 Refnode)
(declare-const z2 Refnode)

(assert
 (and (<= u1 l2)
  (sep
   (sls x l1 z1 u1)
   (pto z1 (c_node y u1))
   (sls y l2 z2 u2)
   (pto z2 (c_node z u2)))
 ))

(assert
 (not
  (sep
    (sls x l1 z2 u2)
    (pto z2 (c_node z u2))
  )
))

(check-sat)
;;expect: unsat
