require 'test/unit'
class BloomFilter_test < Test::Unit::TestCase
  def setup
    @xf = BloomFilter.new(50, 0.01)
    @yf = BloomFilter.new(50, 0.01)
  end
  def test_add
    assert @xf.add("qux")
    assert @xf.hashes == (@xf.bv.inspect.count("1"))
    assert @xf.has?("qux")
    assert @yf.add("foo",["baz"])
    assert @yf.has?("foo")
    assert @yf.has?("baz")
  end
  def test_merge
    assert @xf.add(%w|foo bar baz|)
    assert @yf.add(%w|one two qux|)
    assert zf = @xf | @yf
    %w|foo bar baz one two qux|.each do |key|
      assert zf.has?(key)
    end
    assert @xf.merge(@yf)
    %w|foo bar baz one two qux|.each do |key|
      assert @xf.has?(key)
    end
  end
  def test_intersect
    assert @xf.add(%w|qux bar baz|)
    assert @yf.add(%w|one two qux|)
    assert zf = @xf & @yf
    assert zf.has?("qux")
    %w|bar baz one two|.each do |key|
      assert ! zf.has?(key)
    end
    assert @xf.intersect(@yf)
    assert @xf.has?("qux")
    %w|bar baz one two|.each do |key|
      assert ! @xf.has?(key)
    end
  end
  def test_merge_op
    assert @xf.add(%w|foo bar baz|)
    assert @yf.add(%w|one two qux|)
    assert @xf |= @yf
    %w|foo bar baz one two qux|.each do |key|
      assert @xf.has?(key)
    end
  end
  def test_intersect_op
    assert @xf.add(%w|qux bar baz|)
    assert @yf.add(%w|one two qux|)
    assert @xf &= @yf
    assert @xf.has?("qux")
    %w|bar baz one two|.each do |key|
      assert ! @xf.has?(key)
    end
  end

  def test_equal
    assert @xf.add(%w|foo bar baz|)
    assert @yf.add(%w|foo bar baz|)
    assert @xf == @yf
    assert @yf == @xf
  end

  def test_raise
    assert_raise(ArgumentError){BloomFilter.new(50,1)}
    assert_raise(ArgumentError){BloomFilter.new(50,0)}
    assert_raise(ArgumentError){BloomFilter.new(50,10)}
    assert_raise(RangeError){@xf.add((0..50).to_a)}
    assert_raise(ArgumentError){BloomFilter.new(30,0.01)|@xf}
    assert_raise(ArgumentError){BloomFilter.new(50,0.1)|@xf}
    assert_raise(ArgumentError){BloomFilter.new(50,0.01,['x','y'])|@xf}
    assert_raise(ArgumentError){BloomFilter.new(30,0.01)&@xf}
    assert_raise(ArgumentError){BloomFilter.new(50,0.1)&@xf}
    assert_raise(ArgumentError){BloomFilter.new(50,0.01,['x','y'])&@xf}
  end

  def test_marshal
    assert @xf.add(%w|foo bar baz|)
    zf = Marshal::load(Marshal::dump(@xf))
    assert zf.has?("foo")
    assert zf.has?("bar")
    assert zf.has?("baz")
    assert ! zf.has?("qux")
  end

end
