#!/usr/local/bin/ruby
$:.unshift '.'
require 'test/unit'
require 'test/unit/ui/console/testrunner'

require 'bitset'

class TC_BitSet < Test::Unit::TestCase

  # BitSet.new
  def test_s_new_nil
    a = BitSet.new
    assert_equal(1, a.size)
    assert_equal("0", a.to_s)
  end
  
  def test_s_new_fixnum
    assert_raises(ArgumentError) {
      a = BitSet.new(0)
    }
    
    assert_raises(ArgumentError) {
      a = BitSet.new(-1)
    }
    
    [1,7,8,15,16,255,256,65535,65536].each {|n|
      a = BitSet.new(n)
      assert_equal(n, a.size)
      assert_equal("0" * n, a.to_s)
    }
    
  end
  
  def test_s_new_string
    a = BitSet.new("a")
    assert_equal(8, a.size)
    assert_equal("10000110", a.to_s)
    
    a = BitSet.new("ab")
    assert_equal(16, a.size)
    assert_equal("1000011001000110", a.to_s)
    
    # a\0b
    a = BitSet.new("a\0b")
    assert_equal(24, a.size)
    assert_equal("100001100000000001000110", a.to_s)
  end

  def test_s_new_other
    assert_raises(ArgumentError) {
      a = BitSet.new({})
    }
    
    assert_raises(ArgumentError) {
      a = BitSet.new(:symbol)
    }
    
    assert_raises(ArgumentError) {
      a = BitSet.new(1.0)
    }
  end

  def test_s_new_GC
    ary = Array.new(10)
    assert_nothing_raised {
      200.times do
        idx  = rand(10)
        size = rand(1000000) + 1
        bits = BitSet.new(size)
        ary[idx] = bits
      end
    }
  end

  # BitSet.from_bin
  def test_s_from_bin
    a = BitSet.from_bin("00000")
    assert_equal(5, a.size)
    assert_equal("00000", a.to_s)
    
    a = BitSet.from_bin("0000011111")
    assert_equal(10, a.size)
    assert_equal("0000011111", a.to_s)
  end
  
  # BitSet#to_s
  def test_to_s
    (0..255).each {|i|
      a = BitSet.new(i.chr)
      bit = ("%08b" % i).reverse
      assert_equal(bit, a.to_s)
    }
  end
  
  # BitSet#size
  def test_size_getter
    [1,7,8,15,16,255,256,65535,65536].each {|n|
      a = BitSet.new(n)
      assert_equal(a.size, n)
    }
  end

  # BitSet#size=
  def test_size_setter
    a = BitSet.new()
    [1,7,8,15,16,255,256,65535,65536].each {|n|
      a.size = n
      assert_equal(n, a.size)
    }

    assert_raises(ArgumentError) {
      a.size = 0
    }
    
    assert_raises(ArgumentError) {
      a.size = -1
    }
    
  end

  # BitSet#length
  def test_length
    [1,7,8,15,16,255,256,65535,65536].each {|n|
      a = BitSet.new(n)
      assert_equal(a.length, n)
    }
  end
  
  # BitSet#bytes
  def test_bytes
    [1,7,8,15,16,255,256,65535,65536].each {|n|
      a = BitSet.new(n)
      bytes = (n + 7) / 8;
      assert_equal(a.bytes, bytes)
    }
  end
  
  # BitSet#dup
  def test_dup
    a = BitSet.from_bin("0")
    b = a.dup
    assert_equal("0", b.to_s)
    
    a = BitSet.from_bin("10101010101010101")
    b = a.dup
    assert_equal("10101010101010101", b.to_s)
  end

  # BitSet#clone
  def test_clone
    a = BitSet.from_bin("0")
    b = a.clone
    assert_equal("0", b.to_s)
    
    a = BitSet.from_bin("10101010101010101")
    b = a.clone
    assert_equal("10101010101010101", b.to_s)
  end
  
  # BitSet#get
  def test_get
    a = BitSet.from_bin("1")
    assert_equal(1, a.get(0))
    assert_equal(0, a.get(1))
    assert_equal(0, a.get(100))
    
    a = BitSet.from_bin("10"*50)
    assert_equal(1, a.get(98))
    assert_equal(0, a.get(99))
  end
  
  # BitSet#set
  def test_set
    a = BitSet.new
    a.set 1,1
    assert_equal("01", a.to_s)
    a.set 5,1
    assert_equal("010001", a.to_s)
    a.set 6,1
    assert_equal("0100011", a.to_s)
    a.set 5,0
    assert_equal("0100001", a.to_s)
  end
  
  # BitSet#on
  def test_on
    a = BitSet.new
    a.on 3
    assert_equal("0001", a.to_s)
    a.on 9
    assert_equal("0001000001", a.to_s)
  end
  
  # BitSet#on range
  def test_on_range
    a = BitSet.new
    a.on 1..3
    assert_equal("0111", a.to_s)
    a.on 6..10
    assert_equal("01110011111", a.to_s)
  end
  
  # BitSet#off
  def test_off
    a = BitSet.from_bin("11111111111111111111")
    a.off 5
    assert_equal("11111011111111111111", a.to_s)
    a.off 9
    assert_equal("11111011101111111111", a.to_s)
  end
  
  # BitSet#off
  def test_off_range
    a = BitSet.from_bin("11111111111111111111")
    a.off 2..4
    assert_equal("11000111111111111111", a.to_s)
    a.off 6..10
    assert_equal("11000100000111111111", a.to_s)
  end
  
  # BitSet#clear
  def test_clear
    a = BitSet.from_bin("0")
    a.clear
    assert_equal(1, a.size)
    assert_equal("0", a.to_s)
    
    a = BitSet.from_bin("1111")
    a.clear
    assert_equal(4, a.size)
    assert_equal("0000", a.to_s)
    
    a = BitSet.from_bin("111100001111000011")
    a.clear
    assert_equal(18, a.size)
    assert_equal("000000000000000000", a.to_s)
  end

  # BitSet#|
  def test_or
    a = BitSet.from_bin("000000")
    b = BitSet.from_bin("111000")
    x = a | b
    assert_equal("000000", a.to_s)
    assert_equal("111000", b.to_s)
    assert_equal("111000", x.to_s)
    
    a = BitSet.from_bin("0101010101")
    b = BitSet.from_bin("1111100000")
    x = a | b
    assert_equal("0101010101", a.to_s)
    assert_equal("1111100000", b.to_s)
    assert_equal("1111110101", x.to_s)
    
    a = BitSet.from_bin("00000000000001")
    b = BitSet.from_bin("00000000000010")
    x = a | b
    assert_equal("00000000000011", x.to_s)
    
    a = BitSet.from_bin("1")
    b = BitSet.from_bin("000000001")
    x = a | b
    assert_equal("100000001", x.to_s)

    a = BitSet.from_bin("000000001")
    b = BitSet.from_bin("1")
    x = a | b
    assert_equal("100000001", x.to_s)
  end
  
  # BitSet#+
  def test_plus
    a = BitSet.from_bin("000000")
    b = BitSet.from_bin("111000")
    x = a + b
    assert_equal("000000", a.to_s)
    assert_equal("111000", b.to_s)
    assert_equal("111000", x.to_s)
    
    a = BitSet.from_bin("0101010101")
    b = BitSet.from_bin("1111100000")
    x = a + b
    assert_equal("0101010101", a.to_s)
    assert_equal("1111100000", b.to_s)
    assert_equal("1111110101", x.to_s)
    
    a = BitSet.from_bin("00000000000001")
    b = BitSet.from_bin("00000000000010")
    x = a + b
    assert_equal("00000000000011", x.to_s)
    
    a = BitSet.from_bin("1")
    b = BitSet.from_bin("000000001")
    x = a + b
    assert_equal("100000001", x.to_s)

    a = BitSet.from_bin("000000001")
    b = BitSet.from_bin("1")
    x = a + b
    assert_equal("100000001", x.to_s)
  end

  # BitSet#-
  def test_minus
    a = BitSet.from_bin("01010101")
    b = BitSet.from_bin("00001111")
    x = a - b
    assert_equal("01010101", a.to_s)
    assert_equal("00001111", b.to_s)
    assert_equal("01010000", x.to_s)
    
    a = BitSet.from_bin("111111111111111111111111")
    b = BitSet.from_bin("111111111")
    x = a - b
    assert_equal("000000000111111111111111", x.to_s)
    x = b - a
    assert_equal("000000000", x.to_s)
  end

  # BitSet#<=>
  def test_cmp
    a = BitSet.from_bin("0")
    b = BitSet.from_bin("1")
    assert_equal(-1, a <=> b)
    assert_equal(0, a <=> a)
    assert_equal(1, b <=> a)
    
    a = BitSet.from_bin("1000000000")
    b = BitSet.from_bin("1")
    assert_equal(0, a <=> b)
  end

  # BitSet#&
  def test_and
    a = BitSet.from_bin("010111")
    b = BitSet.from_bin("001110")
    x = a & b
    assert_equal("010111", a.to_s)
    assert_equal("001110", b.to_s)
    assert_equal("000110", x.to_s)

    a = BitSet.from_bin("01010101")
    b = BitSet.from_bin("00001111")
    x = a & b
    assert_equal("01010101", a.to_s)
    assert_equal("00001111", b.to_s)
    assert_equal("00000101", x.to_s)

    a = BitSet.from_bin("0")
    b = BitSet.from_bin("1111111111")
    x = a & b
    assert_equal("0", x.to_s)
    x = b & a
    assert_equal("0000000000", x.to_s)
    
    a = BitSet.from_bin("11111111")
    b = BitSet.from_bin("1111111111111111")
    x = a & b
    assert_equal("11111111", x.to_s)
    x = b & a
    assert_equal("1111111100000000", x.to_s)
    
    a = BitSet.from_bin("11001100")
    b = BitSet.from_bin("1110111011101110")
    x = a & b
    assert_equal("11001100", x.to_s)
    x = b & a
    assert_equal("1100110000000000", x.to_s)
  end

  # BitSet#^
  def test_xor
    a = BitSet.from_bin("01010101")
    b = BitSet.from_bin("00110011")
    x = a ^ b
    assert_equal("01010101", a.to_s)
    assert_equal("00110011", b.to_s)
    assert_equal("01100110", x.to_s)
    
    a = BitSet.from_bin("0")
    b = BitSet.from_bin("10101010101010")
    x = a ^ b
    assert_equal("10101010101010", x.to_s)
    x = b ^ a
    assert_equal("10101010101010", x.to_s)
  end

  # BitSet#*
  def test_multi
    a = BitSet.from_bin("010111")
    b = BitSet.from_bin("001110")
    x = a * b
    assert_equal("010111", a.to_s)
    assert_equal("001110", b.to_s)
    assert_equal("000110", x.to_s)

    a = BitSet.from_bin("01010101")
    b = BitSet.from_bin("00001111")
    x = a * b
    assert_equal("01010101", a.to_s)
    assert_equal("00001111", b.to_s)
    assert_equal("00000101", x.to_s)

    a = BitSet.from_bin("0")
    b = BitSet.from_bin("1111111111")
    x = a * b
    assert_equal("0", x.to_s)
    x = b * a
    assert_equal("0000000000", x.to_s)
    
    a = BitSet.from_bin("11111111")
    b = BitSet.from_bin("1111111111111111")
    x = a * b
    assert_equal("11111111", x.to_s)
    x = b * a
    assert_equal("1111111100000000", x.to_s)
    
    a = BitSet.from_bin("11001100")
    b = BitSet.from_bin("1110111011101110")
    x = a * b
    assert_equal("11001100", x.to_s)
    x = b * a
    assert_equal("1100110000000000", x.to_s)
  end
  
  # BitSet#~
  def test_not
    a = BitSet.from_bin("0")
    x = ~a
    assert_equal("0", a.to_s)
    assert_equal("1", x.to_s)
    
    a = BitSet.from_bin("0101")
    x = ~a
    assert_equal("1010", x.to_s)
    
    a = BitSet.from_bin("00000000001111111111")
    x = ~a
    assert_equal("11111111110000000000", x.to_s)
  end

  # BitSet#zero?
  def test_zero?
    a = BitSet.from_bin("0")
    assert_equal(true, a.zero?)
    a = BitSet.from_bin("0"*100)
    assert_equal(true, a.zero?)
    
    a = BitSet.from_bin("1")
    assert_equal(false, a.zero?)
    a = BitSet.from_bin("0"*100 + "1")
    assert_equal(false, a.zero?)
    
  end
  
  # BitSet#nonzero?
  def test_nonzero?
    a = BitSet.from_bin("0")
    assert_equal(false, a.nonzero?)
    a = BitSet.from_bin("0"*100)
    assert_equal(false, a.nonzero?)
    
    a = BitSet.from_bin("1")
    assert_equal(true, a.nonzero?)
    a = BitSet.from_bin("0"*100 + "1")
    assert_equal(true, a.nonzero?)
  end
  
  # BitSet#max
  def test_max
    a = BitSet.from_bin("0")
    assert_equal(nil, a.max)
    a = BitSet.from_bin("1")
    assert_equal(0, a.max)
    a = BitSet.from_bin("11")
    assert_equal(1, a.max)
    a = BitSet.from_bin("10000000001")
    assert_equal(10, a.max)
  end
  
  # BitSet#min
  def test_min
    a = BitSet.from_bin("0")
    assert_equal(nil, a.min)
    a = BitSet.from_bin("1")
    assert_equal(0, a.min)
    a = BitSet.from_bin("11")
    assert_equal(0, a.min)
    a = BitSet.from_bin("0000100001")
    assert_equal(4, a.min)
  end

  # BitSet#normalize
  def test_normalize
    a = BitSet.from_bin("0")
    x = a.normalize
    assert_equal("0", x.to_s)
    a = BitSet.from_bin("00000")
    x = a.normalize
    assert_equal("0", x.to_s)
    a = BitSet.from_bin("0101010101010000000")
    x = a.normalize
    assert_equal("010101010101", x.to_s)
    assert_equal("0101010101010000000", a.to_s)
  end
  
  # BitSet#normalize!
  def test_normalize!
    a = BitSet.from_bin("0")
    x = a.normalize!
    assert_equal("0", a.to_s)
    assert_equal("0", x.to_s)
    a = BitSet.from_bin("00000")
    x = a.normalize!
    assert_equal("0", a.to_s)
    assert_equal("0", x.to_s)
    a = BitSet.from_bin("0101010101010000000")
    x = a.normalize!
    assert_equal("010101010101", x.to_s)
    assert_equal("010101010101", a.to_s)
  end
  
  # BitSet#to_ary
  def test_to_ary
    a = BitSet.from_bin("00000")
    assert_equal([], a.to_ary)
    a = BitSet.from_bin("01010")
    assert_equal([1,3], a.to_ary)
    a = BitSet.from_bin("1100110101")
    assert_equal([0..1, 4..5, 7, 9], a.to_ary)
    a = BitSet.from_bin("00111111111111011111111111100000000001111111111")
    assert_equal([2..13, 15..26, 37..46], a.to_ary)
  end
  
  # BitSet#each
  def test_each
    a = BitSet.from_bin("010101011")
    ary = []
    a.each {|idx|
      ary << idx
    }
    assert_equal([1, 3, 5, 7, 8], ary)
  end

  # BitSet#to_bytes
  def test_to_bytes
    a = BitSet.new("abc")
    assert_equal("abc", a.to_bytes)
    a = BitSet.from_bin("00000000000000000")
    assert_equal("\x00\x00\x00", a.to_bytes)
  end
  
end


if $0==__FILE__
  Test::Unit::UI::Console::TestRunner.run(TC_BitSet.suite)
end
