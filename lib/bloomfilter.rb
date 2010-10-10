# = bloomfilter.rb: 
# 
# BloomFilter includes a simple but slow SimpleVector bit-vector
# library. However, the BitSet extension library on the RAA is
# recommended for any serious usage:
# 
# * BitSet -- C extension availabe on the RAA
#    (http://raa.ruby-lang.org/project/bitset/)
#
# Copyright (c) 2006 Andrew L. Johnson. Released under that same
# terms as Ruby itself
#
# This implementation uses Algorithm 2 in:
# {Bloom Filters in Probabilistic Verification}[http://www-static.cc.gatech.edu/fac/Pete.Manolios/research/bloom-filters-verification.html]
# (Peter C. Dillinger & Panagiotis Manolios 
# FMCAD 2004, <em>Formal Methods in Computer-Aided Design</em>, 2004)
# This algorithm sets bits with k cycles of two hashes instead of 
# computing k separate hashes.

# The "Which bit-lib?" Waterfall:
begin
  require 'bitset'
  BfVector = BitSet
rescue LoadError
  puts "No BitSet: Resorting to SimpleVector"
  require 'bloomfilter/simplevector'
  BfVector = SimpleVector
end

require 'digest/md5'

class BloomFilter

  Digest = (Digest::MD5).method(:hexdigest)
  Log    = Math.method(:log)

  attr_accessor :capacity, :error, :salts, :bits, :hashes
  attr_accessor :nkeys, :bv
  
  # capacity:: number of elements/keys to add (integer)
  # error::    error rate ( 0.0 < error < 1.0)
  # salts::    optionally specify array of two salt-strings
  #
  def initialize(capacity, error, salts=["42","fortytwo"])
    raise ArgumentError, "Error must lie: 0.0 < error < 1.0" if
      error <= 0.0 or error >= 1.0
    @capacity = capacity
    @error    = error
    @nkeys    = 0
    @bits     = (capacity * Log[error] / Log[1.0 / 2**Log[2]]).ceil
    @hashes   = (Log[2] * bits / capacity).round
		# Was - ryan
		@bv       = BfVector.new(bits)
    
		# changed to - ryan
    #@bv       = BfVector.new((bits / 8))
    @salts    = salts
  end

  # add list or array of keys to filter
  def add(*keys)
    keys.flatten.each do |key|
      raise RangeError, "Too many keys" if (self.nkeys += 1) > capacity
#      bits_on(key) {|pos| self.bv[pos] = 1}
      bits_on(key) {|pos| self.bv.set(pos, 1)}
    end
  end
  alias :insert :add

  # call-seq:
  #   has?(key)  -> true or false
  #   
  def has?(key)
    bits_on(key){|pos| self.bv.get(pos) == 1 or return false}
    true
  end

  # yields each bit-position for a given key
  def bits_on(key)
    pos1, pos2 = salts.map{|s|Digest[s + key.to_s].hex % bits}
    hashes.times do |i|
      yield pos1
      pos1 = (pos1 + pos2) % bits
      pos2 = (pos2 + i)  % bits
    end
  end
  private :bits_on

  # destructively merge another bfilter into self (must be same
  # size and have been created with the same salts)
  def merge(bfilter)
    raise ArgumentError unless self === bfilter
    self.bv |= bfilter.bv
    self
  end
  
  # call-seq:
  #   bfilter | otherfilter   
  #   
  # non-destructive merge (same constraints as merge())
  def |(bfilter)
    nf = self.dup.merge(bfilter)
  end

  # destructive intersection with another filter (same constraints as merge)
  def intersect(bfilter)
    raise ArgumentError unless self === bfilter
    self.bv &= bfilter.bv
    self
  end

  # call-seq:
  #   bfilter & otherfilter
  #   
  # non-destructive intersection (constraints as merge)
  def &(bfilter)
    nf = self.dup.intersect(bfilter)
  end

  # call-seq:
  #   bfilter === otherfilter
  #
  # test equality of vector size and salts and number of hashes
  def ===(bfilter)
    self.bits == bfilter.bits   and 
    self.salts == bfilter.salts and
    self.hashes == bfilter.hashes
  end

  # call-seq:
  #   bfilter == otherfilter
  #   
  def ==(bfilter)
    Marshal::dump(self) == Marshal::dump(bfilter)
  end

  # called on Marshal.dump(bfilter)
  def marshal_dump
    {:capacity => self.capacity, :error => self.error,
     :salts => self.salts, :bits => self.bits,
     :hashes => self.hashes, :nkeys => self.nkeys,
     :bv => self.bv.store}
  end

  # called on Marshal.load(dumped_filter)
  def marshal_load(obj)
    obj.each do |k,v|
      v = BfVector.restore(v,obj[:bits]) if k == :bv
      self.send("#{k}=",v)
    end
  end

end

# Add store/restore to BitSet for marshal/load purposes:
class BitSet
  def store
    to_bytes
  end
  def BitSet.restore(bfstr,n)
    nb = BitSet.new(bfstr)
    nb.size = n
    nb
  end
end
__END__


