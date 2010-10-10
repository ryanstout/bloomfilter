# This is a slower and more limited bitvector class. 
# Please use the BitSet C-extension library from below instead:
# *  http://raa.ruby-lang.org/project/bitset/

class SimpleVector
  attr_accessor :bvec, :length
  alias :size :length

  # call-seq:
  #   SimpleVector.new(bits)
  #   
  def initialize(arg)
    @length = arg
    @bvec = 1<<@length
  end

  def SimpleVector.from_str(n,hexstr)
    bv = SimpleVector.new(n)
    bv.bvec = hexstr.hex
    bv
  end

  def empty
    @bvec = 1 << size
  end
  alias :clear :empty
  
  def to_str
    @bvec.to_s(16)
  end

  def union(other)
    self.bvec |= other.bvec
    self
  end
  def |(other)
    self.dup.union(other)
  end

  def intersect(other)
    self.bvec &= other.bvec
    self
  end

  def &(other)
    self.dup.intersect(other)
  end

  def ==(other)
    self.bvec == other.bvec
  end

  def []=(pos,val=1)
    case val
      when 1: @bvec |= 1<<pos
      when 0: @bvec &= ~(1<<(length+1) | 1<<pos)
    end
    self
  end

  def [](pos)
    (@bvec & 1<<pos ) == 0 ? 0 : 1
  end

  def inspect
    (@bvec).to_s(2)[1..-1]
  end

  ## store/restore to/from BitSet style bytestrings

  def store
    sv = self.bvec & ~(1<<(length+1) | 1<<size)
    sv = sv.to_s(16)
    sv[0,0] = '0' * (((size+7)/8) * 2 - sv.size)
    [sv].pack('H*').reverse
  end

  def SimpleVector.restore(bfstr,n)
    bytes = bfstr.size
    sv = bfstr.reverse.unpack("H*")[0]
    sv = sv.hex | 1<<n
    nv = SimpleVector.new(n)
    nv.bvec = sv
    nv
  end
end

__END__

