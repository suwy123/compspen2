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
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const v2 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const v16 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)

(assert(and
(= x1 x2) (<= x2 x20) (< x20 x3) (= x8 (+ x6 1)) (= x10 (+ x8 1)) (= x11 x12)
(= x13 x14) (= x15 x16) (= x17 x18)
(sep
(blk x0 x1)
(hls x2 x3 v2)
(hls x4 x5 v4)
(pto x6 (c_data_t x7))
(pto x8 (c_data_t x9))
(blk x10 x11)
(blk x12 x13)
(blk x14 x15)
(hls x16 x17 v16)
(pto x18 (c_data_t x19))
)))

(assert(not
(sep
(blk x0 x20)
(blk x20 x3)
(hls x4 x5 v4)
(blk x6 (+ x18 1))
)))

(check-sat)