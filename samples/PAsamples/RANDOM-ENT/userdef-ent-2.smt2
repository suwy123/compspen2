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
(define-fun hck ((?x Int) (?b1 Int) (?y Int) (?b2 Int) (?v Int)) Bool
	(and
		(or
			(and (>= (- ?y ?x) 4) (<= (- ?y ?x) ?v) (= ?b1 0) (= ?b2 1))
			(and (>= (- ?y ?x) 4) (<= (- ?y ?x) ?v) (= ?b1 1) (= ?b2 0))
		)
		(sep
				(pto ?x (c_data_t (- ?y ?x)))
				(pto (+ ?x 1) (c_data_t ?b2))
				(blk (+ ?x 2) ?y)
		)
	)
)
;; heap list
(define-fun-rec hls ((?x Int) (?b1 Int) (?y Int) (?b2 Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (= ?b1 ?b2) (_ emp Int data_t))
		(exists ((?w Int) (?st Int))
			(sep
				(hck ?x ?b1 ?w ?st ?v)
				(hls ?w ?st ?y ?b2 ?v)
		))
	)
)


;; variables
(declare-const x0 Int)
(declare-const sz0 Int)
(declare-const x1 Int)

(declare-const x2 Int)
(declare-const x3 Int)

(declare-const x4 Int)
(declare-const x5 Int)

(declare-const b1 Int)
(declare-const b2 Int)
(declare-const b3 Int)
(declare-const b4 Int)

(declare-const v Int)

(assert
	(and
	(= x3 x4)
	(= b2 b3)
	(sep
		(pto x0 (c_data_t sz0))
		(blk (+ x0 1) x1)
		(hck x2 b1 x3 b2 v)
		(hls x4 b3 x5 b4 v)
	)
	)	)
(assert (not
	(sep
		(blk x0 x1)
		(hls x2 b1 x5 b4 v)
	)
))
;; end of problem
(check-sat)
;; 5.55s unsat:the entl is true
