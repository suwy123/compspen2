(set-logic QF_SLAH)

(declare-datatypes (
	(data_t 0)
	) (
	((c_data_t (sz Int)))
	)
)


(declare-heap (Int data_t) 
)


;; heap chunk
(define-fun hck ((?x Int) (?y Int)) Bool
	(exists ((?f Int))
	(and
		(or
			(and (>= (- ?y ?x) 4) (= ?f 0))
			(and (>= (- ?y ?x) 4) (= ?f 1))
		)
		(sep
			(pto ?x (c_data_t (- ?y ?x)))
			(pto (+ ?x 1) (c_data_t ?f))
			(blk (+ ?x 2) ?y)
		)
	)
	)
)
;; heap list
(define-fun-rec hls ((?x Int) (?y Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int))
			(sep
					(hck ?x ?w)
					(hls ?w ?y)
		))
	)
)



;; declare variables
(declare-const t1 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const z1 Int)
(declare-const z2 Int)


(assert
	(and
		(= t1 x2)
		(= x1 z2)
		(sep
			(pto t1 (c_data_t 3))
			(blk x1 x2)
			(hls z1 z2)
		)
	)
)

(assert
	(not
		(and
			(> t1 z1)
			(sep
				(blk z1 z2)
				(blk z2 t1)
			)
		)
	)
)

(check-sat)

;;0.043s sat:the entl is false because abs(A) \vDash abs(B) z2>z1
