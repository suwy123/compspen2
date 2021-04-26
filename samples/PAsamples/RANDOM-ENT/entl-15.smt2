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
(declare-const u1 Int)
(declare-const t2 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const y1 Int)
(declare-const y2 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const z3 Int)


(assert
	(and
		(= z1 z2)
		(= y1 y2)
		(hls y1 y2)
	)
)

(assert
	(not
		(and
			(= y1 y2)
		)
	)
)

(check-sat)
;;0.08s unsat:the entl is true
