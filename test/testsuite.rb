$: << '../lib' << './lib'
Dir.chdir(File.dirname($0))
Dir['tc_*'].each do |t|

  puts "-" * 60
  pid = fork do
    $0 = t
    puts "Testing with C bitset.so"
    begin
      require 'bitset.so'
    rescue LoadError
      puts "Skipping test: no 'bitset.so' installed"
      exit
    end
    require 'bloomfilter'
    Object.class_eval "remove_const(:BfVector)"
    BfVector = BitSet
    require t
  end
  Process.wait(pid)

  puts "-" * 60
  pid = fork do
    $0 = t
    puts "Testing with Ruby bloomfilter/simplevector.rb"
    require 'bloomfilter'
    Object.class_eval "remove_const(:BfVector)"
    load 'bloomfilter/simplevector.rb'
    BfVector = SimpleVector
    require t
  end
  Process.wait(pid)
end
