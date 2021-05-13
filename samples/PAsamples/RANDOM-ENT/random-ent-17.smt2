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
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const v6 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const v8 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const v10 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const v12 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)

(assert(and
(< x0 x1) (= x1 x2) (= x5 x6) (= x7 x8) (< v4 v6) (< v6 v8) (= v10 v12) (= x11 x12)
(< x14 x20) (< x20 x15) (= x17 x21)
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(hls x4 x5 v4)
(hls x6 x7 v6)
(hls x8 x9 v8)
(hls x10 x11 v10)
(hls x12 x13 v12)
(blk x14 x15)
(pto x16 (c_data_t x17))
(blk x18 x19)
)))

(assert(not(and
(distinct x0 x1)
(sep
;(blk x0 (+ x2 1))
(hls x4 x9 v8)
(hls x10 x13 v12)
(blk x14 x20)
(blk x20 x15)
(pto x16 (c_data_t x21))
(blk x18 x19)
))))

(check-sat)