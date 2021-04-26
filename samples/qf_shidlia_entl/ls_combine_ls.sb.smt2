(set-logic QF_SLID_INT)

(declare-sort Refnode 0)

(declare-datatypes
 ((node 0))
 (((c_node (next Refnode)))))

(declare-heap (Refnode node))

;; declare predicates
(define-fun-rec lls 
	((?in Refnode) (?len1 Int) (?out Refnode) (?len2 Int)) 
	Bool
		(or 
			(and (= ?in ?out) (= ?len1 ?len2) 
			(_ emp Refnode node)
			)

 
			(exists 
				((?u Refnode) (?len3 Int)) 
				(and  	
					(= ?len1 (+ ?len3 1))
					(sep 
						(pto ?in (c_node ?u) ) 
						(lls ?u ?len3 ?out ?len2) 
					)

				)
 

			)
		)
)


;(define-fun-rec ls ((x_1 Refnode) (y_2 Refnode) (n_3 Int)) Bool
;  (or
;   (and
;    (_ emp Refnode node)
;    (and
;     (= n_3 0)
;     (= x_1 y_2)))
;   (exists
;    ((u_4 Refnode) (k Int))
;    (and
;     (sep
;      (pto x_1 (c_node u_4))
;      (ls u_4 y_2 k))
;     (= k (+ n_3 (- 1)))
;     (<= 0 (+ n_3 (- 1)))))))

;;;;ls(x,y,n) = lls(x,n,y,0)

;; entailment: ls(x,y,1000) * ls(y,z,1000) |- ls(x,z,2000)

(declare-const x Refnode)
(declare-const y Refnode)
(declare-const z Refnode)
(declare-const k1000 Int)
(declare-const k2000 Int)

(assert
 (and (= k1000 1000) (= k2000 2000)
 (sep
  (lls x k2000 y k1000)
  (lls y k1000 z 0))
 ))

(assert
 (not
  (lls x k2000 z 0)
))

(check-sat)
;;unsat
