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
(declare-const v1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const v6 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const v10 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)

(assert(and
(< v0 v1) (= x4 (+ x2 1)) (= x5 x6) (= x5 (+ x2 x3)) (< x3 v6)
(= x9 x10) (= x11 x12) (= x16 (+ x14 1)) (= x18 (+ x16 1)) (= (+ x14 x15) (+ x18 1))
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(blk x4 x5)
(hls x6 x7 v6)
(blk x8 x9)
(hls x10 x11 v10)
(blk x12 x13)
(pto x14 (c_data_t x15))
(pto x16 (c_data_t x17))
(pto x18 (c_data_t x19))
)))

(assert(not(and
(sep
(hls x0 x1 v1)
(hls x2 x7 v6)
(blk x8 x13)
(hls x14 (+ x18 1) x15)
))))

(check-sat)