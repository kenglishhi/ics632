require 'rubygems'
require 'bio'
require 'pp'
SIMILARITY_MIN = 80.0 
def my_similarity_report(data) 

#   pp data 
  data.sort.each do |seq,records| 
    records.each do | row|
       puts " #{seq} -> #{row[:target]}  [label=\"#{row[:similarity]}\"]  "
    end
#    puts "#{key} ==>   0
  end
#  data = data.sort{|row1,row2| row2['Similarity'].to_f <=> row1['Similarity'].to_f } 
#  data = data.select{|row| row['Similarity'].to_f > 80.0 } 
#  data.each do | row | 
#    puts " #{row['1']} -> #{row['2']}  [label=\"#{row['Similarity']}\" ] "
#  end
#  unless (data['1'] == data['2'] ) 
#    pp data
#  end
end

filename =  ARGV.shift || "fastas/frags.fasta"
exit if (filename.nil? || !File.exists?(filename))

fasta = Bio::FastaFormat.open(filename)


data = {} 
newfilenames =[] 
fasta.each do |entry|
  newfilename = filename.sub(/\.fasta/,"#{entry.definition}.fasta")
  File.open(newfilename, "w") do | f|
    f.puts entry.to_s
    data[entry.definition.to_i] = []
  end
  newfilenames << newfilename
end

newfilenames.each do | newfilename| 
  result = Bio::EMBOSS.run('water', '-asequence', newfilename, '-bsequence', filename)
  current_seq_num = 0 
  target_seq_num = 0 
#  puts result

  result.split("\n").each do | line | 
     if line =~ /^# Aligned_sequences/
      # reset
       current_seq_num = 0 
       target_seq_num = 0 
     end
     # Get sequence numbers 
     if line =~ /^# (\d+): (\d+)/
       current_seq_num  = $2.to_i if $1 == '1' 
       target_seq_num = $2.to_i if $1 == '2' 
#       all_data[$1] ||= [] 
#       all_data[$1] << {:target => $2} 
     end
     # parse similarity
     if line =~ /^# Similarity:.*\((.*)%\)/
        similarity  = $1
        if ((similarity.to_f  > SIMILARITY_MIN && current_seq_num != target_seq_num) && 
             !data[target_seq_num].detect{|row| row[:target] == current_seq_num } ) 
          data[current_seq_num] << {:target => target_seq_num, :similarity   => similarity  } 
#          puts " current_seq_num:  #{current_seq_num} , target_seq_num:  #{target_seq_num}  #{data.inspect} " 
        else 
#          puts " SKIPPING:  cur: #{current_seq_num} , target:  #{target_seq_num} similarity: #{similarity}"  
        end
     end

   end

#   exit if  newfilename == 'fastas/frags4.fasta'
end

   pp data.sort

   my_similarity_report(data)

