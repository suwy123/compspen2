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
	(and
		(>= (- ?y ?x) 2)
		(sep
			(pto ?x (c_data_t (- ?y ?x)))
			(blk (+ ?x 1) ?y)
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
			)
		)
	)
)

;; 256 <= szy &
;; z = (y + szy) &
;; 256 <= szz &
;; u = z + szz 
;; :
;; integer(y,szy+szz) *
;; blk(y+1,z) *
;; integer(z,szz) *
;; blk(z+1,u)
;; 
;; ==>
;; 
;; hck(y, szy+szz, u)

;; declare variables
(declare-const y Int)
(declare-const szy Int)
(declare-const z Int)
(declare-const szz Int)
(declare-const u Int)

(assert (and
		(< 256 szy)
		(= z (+ y szy))
		(< 256 szz)
		(= u (+ z szz))
		(sep
			(pto y (c_data_t (+ szy szz)))
			(blk (+ y 1) z)
			(pto z (c_data_t szz))
			(blk (+ z 1) u)
		)
	)
)

(assert (not
		(sep
			(pto y (c_data_t (+ szy szz)))
			(blk (+ y 1) u)
		)
	)
)

(check-sat)
;; 0.32s unsat : entl is true
