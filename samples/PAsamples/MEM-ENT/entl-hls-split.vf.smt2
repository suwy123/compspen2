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

;; sz + MIN_SIZE <= szy &
;; sz >= MIN_SIZE &
;; nc = y + szy - sz &
;; z = y + szy
;; :
;; integer(y,szy-sz) *
;; blk(y+1,y+szy-sz) *
;; integer(nc, sz) *
;; blk(nc+1,z)
;; 
;; ==>
;; 
;; hck(y, szy-sz,nc) *
;; hck(nc, sz, z)

;; declare variables
(declare-const sz Int)
(declare-const y Int)
(declare-const szy Int)
(declare-const z Int)
(declare-const nc Int)

(assert (and
		(< 256 sz)
		(< 256 szy)
		(< (+ sz 256) szy)
		(= z (+ y szy))
		(= nc (+ y (- szy sz)))
		(sep
			(pto y (c_data_t (- szy sz)))
			(blk (+ y 1) (+ y (- szy sz)))
			(pto nc (c_data_t sz))
			(blk (+ nc 1) z)
		)
	)
)

(assert (not
		(hls y z)
	)
)

(check-sat)
;; 0.88s unsat : entl is true
