(set-logic QF_SLID_INT)

(declare-sort Refnode 0)

(declare-datatypes
 ((node 0))
 (((c_node (next Refnode) (data Int)))))

(declare-heap (Refnode node))

;; declare predicates
(define-fun-rec sls1
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
						(sls1 ?u ?dt3 ?out ?dt2)
					)
				)
			)
		)
)

;(define-fun-rec sls ((x_1 Refnode) (y_2 Refnode) (l_3 Int) (u_4 Int)) Bool
;  (or
;   (and
;    (pto x_1 (c_node y_2 l_3))
;    (= l_3 u_4))
;   (exists
;    ((t_5 Refnode) (a_6 Int))
;    (and
;     (sep
;      (pto x_1 (c_node t_5 l_3))
;      (sls t_5 y_2 a_6 u_4))
;     (and
;      (<= a_6 u_4)
;      (<= l_3 a_6))))))

;;;;sls(x,y,l,u) = \exists z. \wedge sls1(x,l,z,u)*(pto z (sref (ref next y) (ref data u)))

;; entailment: sls(x,y,l1,100) * sls(y,z,101,200) * sls(z,t,201,u3) |- sls(x,t,l1,u3)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const l1 Int)
(declare-const z Refnode)
(declare-const t Refnode)
(declare-const u3 Int)
(declare-const k100 Int)
(declare-const k101 Int)
(declare-const k200 Int)
(declare-const k201 Int)

(declare-const z1 Refnode)
(declare-const z2 Refnode)
(declare-const z3 Refnode)

(assert
 (and (= k100 100) (= k101 101) (= k200 200) (= k201 201)
 (sep
  (sls1 x l1 z1 k100)
  (pto z1 (c_node y k100))
  (sls1 y k101 z2 k200)
  (pto z2 (c_node z k200))
  (sls1 z k201 z3 u3)
  (pto z3 (c_node t u3))
 )
))

(assert
 (not
  (sep
    (sls1 x l1 z3 u3)
    (pto z3 (c_node t u3))
  )
))

(check-sat)
;;unsat
