(set-logic QF_SLAH)

(declare-datatypes (
	(data_t 0)
	) (
	((c_data_t (data Int)))
	)
)


(declare-heap (Int data_t) 
)

;; heap chunk
(define-fun hck ((?x Int) (?y Int) (?v Int)) Bool
	(exists ((?f Int))
	(and
		(or
			(and (>= (- ?y ?x) 3) (= ?f 0))
			(and (>= (- ?y ?x) 3) (<= (- ?y ?x) ?v) (= ?f 1))
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
(define-fun-rec hls ((?x Int) (?y Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int))
			(sep
					(hck ?x ?w ?v)
					(hls ?w ?y ?v)
		))
	)
)

;; variables
(declare-const x0 Int)
(declare-const sz0 Int)
(declare-const x1 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const v Int)

(assert
	(and	
		(= sz0 (- z1 x0))
		(= z0 x0)
		(sep
			(pto x0 (c_data_t sz0))
			(pto (+ x0 1) (c_data_t 0))
			(blk (+ x0 2) z1)
		)
	)	
)

(assert
	(not
		(hls z0 z1 v)	
	)
)
;; end of problem
(check-sat)
;; 0.78s unsat:the entl is true
