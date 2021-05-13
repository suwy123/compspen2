(set-logic QF_SLAH)

(declare-datatypes ((data_t 0)) (((c_data_t (sz Int)))))

(declare-heap (Int data_t))

(define-fun hck ((?x Int) (?y Int) (?v Int)) Bool
	(and
		(>= (- ?y ?x) 2) (<= (- ?y ?x) ?v)
		(sep (pto ?x (c_data_t (- ?y ?x))) (blk (+ ?x 1) ?y))
))

(define-fun-rec hls ((?x Int) (?y Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int)) (sep (hck ?x ?w ?v) (hls ?w ?y ?v)))
))

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const v0 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)

(assert
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(pto x4 (c_data_t x5))
(pto x6 (c_data_t x7))
(pto x8 (c_data_t x9))
))

(assert(not
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(blk x4 (+ x4 1))
(pto x6 (c_data_t x7))
(blk x8 (+ x8 1))
)))

(check-sat)