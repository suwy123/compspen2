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
(declare-const x10 Int)
(declare-const v8 Int)

(assert(and
(= x1 x2) (= (+ x2 1) x4) (= (+ x2 x3) x10) (< 2 x3)
(< x3 v0) (< x4 x10) (< x10 x5) (= x5 x6) (= (+ x6 1) x8)
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(blk x4 x5)
(pto x6 (c_data_t x7))
(hls x8 x9 v8)
)))

(assert(not(and
(= x1 x2)
(sep
(hls x0 x10 v0)
(blk x10 x9)
))))

(check-sat)