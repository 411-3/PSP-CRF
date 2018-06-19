
import subprocess, numpy

print "\n*********Output**********\n"
# read list_name 
pic_list = []
fr = open('/home/xduser/lihuan/superpixel_crf/val.txt')
test_pic_num = 20
#fr = open('/home/xduser/lihuan/superpixel_crf/pic/loss_fcn.txt')
#test_pic_num = 22

for i in range(test_pic_num):
	linestr = fr.readline()
	linestr = linestr.strip('\n')
	pic_list.append(linestr)
	
exename  = '/home/xduser/lihuan/superpixel_crf/superpixel_crf_acc'
jpgname0 = '/home/xduser/lihuan/pascal2012/jpeg/JPEGImages/'

scorename1 = '/media/xduser/D/PASCAL2012/deeplab_out/'
scorename2 = '/home/xduser/lihuan/pascal2012/fcn/out/'

outname0 = '/home/xduser/lihuan/superpixel_crf/pic/0616/acc/'

model = 0 # 0->3; 1->2_modified;
ifbibao = 1
anb = 12

for w1 in [3]:
	for theta in [30]:
		for beta in [2]:
			for w3 in [3]:
				for delta in [2]:
					for j in range(test_pic_num):
						args = []
						jpgname = jpgname0 + pic_list[j] + '.jpg'
						scorename = scorename2 + pic_list[j] + '.jpg.score.npy'
						outname = outname0 +'pAfcn_0516_noentropy'+'/'+ pic_list[j]+'.png'
						args.append(exename)
						args.append(jpgname)
						args.append(scorename)
						args.append(outname)
						args.append('%d' %model )
						args.append('%d' %w1    )
						args.append('%d' %theta )
						args.append('%d' %beta  )
						args.append('%d' %w3    )
						args.append('%d' %delta )
						args.append('%d' %ifbibao )
						args.append('%d' %anb )
						# call crf.exe
						try:
							child = subprocess.Popen(args)
							child.wait()
						except:
							print "subprocess error!\n"
						del args[:]
						jpgname = fcnname = outname = ''
						#print "--------------------------------%d_%d_%d_%d_%d_params has been tested!" % w1,theta,beta,w3,delta
print "****************All w has been tested!\n"			

