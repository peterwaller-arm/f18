! Test numeric model inquiry intrinsics

module m

  integer, parameter :: &
    bs1 = bit_size(0_1), &
    bs2 = bit_size(0_2), &
    bs4 = bit_size(0_4), &
    bs8 = bit_size(0_8), &
    bs16 = bit_size(0_16)
  logical, parameter :: test_bit_size_1 = bs1 == 8
  logical, parameter :: test_bit_size_2 = bs2 == 16
  logical, parameter :: test_bit_size_4 = bs4 == 32
  logical, parameter :: test_bit_size_8 = bs8 == 64
  logical, parameter :: test_bit_size_16 = bs16 == 128

  real(2), parameter :: &
    eps2 = epsilon(0._2), zeps2 = real(z'1000', kind=2), deps2 = 4.8828125e-4_2
  real(3), parameter :: &
    eps3 = epsilon(0._3), zeps3 = real(z'3b80', kind=3), deps3 = 3.90625e-3_3
  real(4), parameter :: &
    eps4 = epsilon(0._4), zeps4 = real(z'33800000', kind=4), &
    deps4 = 5.9604644775390625e-8_4
  real(8), parameter :: &
    eps8 = epsilon(0._8), zeps8 = real(z'3ca0000000000000', kind=8), &
    deps8 = 1.1102230246251565404236316680908203125e-16_8
  real(10), parameter :: &
    eps10 = epsilon(0._10), zeps10 = real(z'3fbf8000000000000000', kind=10), &
    deps10 = 5.42101086242752217003726400434970855712890625e-20_10
  real(16), parameter :: &
    eps16 = epsilon(0._16), &
    zeps16 = real(z'3fc78000000000000000000000000000', kind=16), &
    deps16 = 1.925929944387235853055977942584927318538101648215388195239938795566558837890625e-34_16
  logical, parameter :: test_eps2 = eps2 == zeps2 .and. eps2 == deps2
  logical, parameter :: test_eps3 = eps3 == zeps3 .and. eps3 == deps3
  logical, parameter :: test_eps4 = eps4 == zeps4 .and. eps4 == deps4
  logical, parameter :: test_eps8 = eps8 == zeps8 .and. eps8 == deps8
  logical, parameter :: test_eps10 = eps10 == zeps10 .and. eps10 == deps10
  logical, parameter :: test_eps16 = eps16 == zeps16 .and. eps16 == deps16

  integer(1), parameter :: &
    ihuge1 = huge(0_1), zihuge1 = int(z'7f', kind=1), dihuge1 = 127_1
  integer(2), parameter :: &
    ihuge2 = huge(0_2), zihuge2 = int(z'7fff', kind=2), dihuge2 = 32767_2
  integer(4), parameter :: &
    ihuge4 = huge(0_4), zihuge4 = int(z'7fffffff', kind=4), &
    dihuge4 = 2147483647_4
  integer(8), parameter :: &
    ihuge8 = huge(0_8), zihuge8 = int(z'7fffffffffffffff', kind=8), &
    dihuge8 = 9223372036854775807_8
  integer(16), parameter :: &
    ihuge16 = huge(0_16), &
    zihuge16 = int(z'7fffffffffffffffffffffffffffffff', kind=16), &
    dihuge16 = 170141183460469231731687303715884105727_16
  logical, parameter :: test_ihuge1 = ihuge1 == zihuge1 .and. ihuge1 == dihuge1
  logical, parameter :: test_ihuge2 = ihuge2 == zihuge2 .and. ihuge2 == dihuge2
  logical, parameter :: test_ihuge4 = ihuge4 == zihuge4 .and. ihuge4 == dihuge4
  logical, parameter :: test_ihuge8 = ihuge8 == zihuge8 .and. ihuge8 == dihuge8
  logical, parameter :: test_ihuge16 = ihuge16 == zihuge16 .and. ihuge16 == dihuge16

  real(2), parameter :: &
    ahuge2 = huge(0._2), zahuge2 = real(z'7bff', kind=2), dahuge2 = 6.5504e4_2
  real(3), parameter :: &
    ahuge3 = huge(0._3), zahuge3 = real(z'7f7f', kind=3), &
    dahuge3 = 3.3895313892515354759047080037148786688e38_3
  real(4), parameter :: &
    ahuge4 = huge(0._4), zahuge4 = real(z'7f7fffff', kind=4), &
    dahuge4 = 3.4028234663852885981170418348451692544e38_4
  real(8), parameter :: &
    ahuge8 = huge(0._8), zahuge8 = real(z'7fefffffffffffff', kind=8), &
    dahuge8 = 1.7976931348623157081452742373170435679807056752584499659891747680315726078002853876058955863276687817&
               &1540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868&
               &5084551339423045832369032229481658085593321233482747978262041447231687381771809192998812504040261841&
               &24858368e308_8
  real(10), parameter :: &
    ahuge10 = huge(0._10), zahuge10 = real(z'7ffeffffffffffffffff', kind=10), &
    dahuge10 = 1.1897314953572317650212638530309702051690633222946242004403237338917370055229707226164102903365288828&
                &5354569780749557731442744315367028843419812557385374367867359320070697326320191591828296152436552951&
                &0646791086614311790632169778838896134786560600399148753433211454911160088679845154866512852340149773&
                &0376000091254793939662231513836224178385427439178381387178058894875405751682263476592355769748051137&
                &2564902088485522249479139937758502601177354918009979622602685950855888360815984690023564513234659447&
                &6384939859276456284579661772930407806609229102715046085388087959327781622986827547830768080040150694&
                &9423034117289577771003357140105597752421240573470073862516601108283791196230084692772009651535002084&
                &7447079244384854591288672300061908512647211195136146752763351956292759795725027800298079590419313960&
                &3021470997035276467445530922022679656280991498232083329641241038509239184734786121921697210543484287&
                &0483534081130425730022164213489173471742348007148807510020643905172342476560047217680964861079949434&
                &1570347632064355862420744350442438056613601760883747816538902780957697597728686007148702828795556714&
                &1404632615832623602762896316173978484254486860609948270867968048078702511858930838546584223040908805&
                &9962945945862019037660484467909260022254105307759010657606713472001258464069570302571389609837579989&
                &2695455305236856075868317922311363951946885088077187210470520395758748001314313144425494391994017575&
                &3169339392366881856189129931729104252921236835159922322050998001677102784035360140829296398115122877&
                &7681357060457893435354516965395612540488464471697868932116710872290880827783505182288576460622187397&
                &0285165508372099234948333443522898475123275372663606621390228126470623407535207172405866507951821730&
                &3463782631353393706774901950197841690441824738063162828586857741432581165364040218402724913393320949&
                &2194984224427304270198730445366203502623869578046820036014472919971230955300572061418669748528468561&
                &8651483271597448120312194675168637934309618961510733006555242148519520176285859509105183947250286387&
                &1632494167613804996319791441870254302706758495192008837915169401581740046711477877201459644461175204&
                &0594535047647218079757611117208462736392796003396704700376133745095531841500737964126050479232516613&
                &5484129188421134082301547330475406707281876350361733290800595189632520707167390454777712968226520622&
                &5651439919376804400292380903112437912614776255964694221981375146967079446870358004392507659451618379&
                &8118593920495440361149153107822510726914869798092409467721427270124043771874092167566136349389004512&
                &3235166814608932240069799317601780533819184998193300841098599393876029260139091141452600372028487213&
                &2411955424282101831204216104467404621635336900583664606591156298764745525068145003932941404131495400&
                &6776029510059622530228230036314738246810596484424413248645731374375950964161680480241293518762046681&
                &3563687753281467553879887177183651289394719533506188500326760735438867336800207438784965701457609034&
                &9857571243045102038730494854256702479339322809110526041538528994849203991091946129912491633289917998&
                &0943803378795220931314669461497059396641523759492858909604899161219449899863848370224866722491489246&
                &7841020618336462741696957630763248023558797524525373703543388296086275342774001633343405508353704850&
                &7374544819754722228975281083020898682633020285259923084168054539687911418297629988964576482765287504&
                &5628549242651652177507995162596692291149777889623566709566271384820181913483216879958636526376209782&
                &8507009933729439678463987902491451422274252700636394232799848397673998715441855420156224415492665301&
                &4515504685489258620276085761837129763358761215382565129633538141663949516556000264159186554850057052&
                &6114319529199188079545223946496276356301785808966922264062353828985358675959906470083856871238103295&
                &9192649484625076899225841930548076362021508902214922052806984201835084058693849381549890944546197789&
                &3029113576516775406232278298314033473276603952231603422824717528181818844304880921321933550869873395&
                &8612760736708666523755556758031714901084773200964243187800700087973460329062789435537435644488519071&
                &9161645514115576193939969076741515640282654366402676009508752394550734155613586793306603174472092444&
                &6513532366647649735400851967040771103640538150073486891798364049570606189535005089840913826869535090&
                &0667833244725787121966044152849248400418509328119089636341757398971665960007594878006191640948543387&
                &5852065711654107226099628815012314437794400874930194474433078438899570184271000480830501217712356062&
                &2895076269042856800047718893158089358515593863176652948089031267747029662545110861548958395087796755&
                &4641379448959605279752098748138397625785921057562844017593493241621483395653501891968113890918437957&
                &3470326940634289008780584694035245347939808067427323629788710086717580253156130235606487870925986528&
                &8416350972529537091114317204887747405539054009425375424119317944175137064689643861517718849867010341&
                &5325423859110896247108853858086888377772586485641459342621210866475884892600317623459607695088491496&
                &6244415660441955208681198977024e4932_10
  real(16), parameter :: &
    ahuge16 = huge(0._16), zahuge16 = real(z'7fff7fffffffffffffffffffffffffff', kind=16), &
    dahuge16 = 1.4154610310449547890015530277449513287402522312014583583863873452370673496090517844595674241105526619&
                &5316741942296713234339847173043444610109053451307461326030568532269393756935971597477179751917262481&
                &9352114171249119710791528826008514763128252507906157761158330471593133360520566465839598159704790361&
                &8052216473233827726396998249642738685081537609877654130679009320149945501056683481052234280369184890&
                &5600784088324331956396165189067350444857059625918182061259630449033723034922117505897765689572130343&
                &6442900178897829054258075776096285494374231192475505398864394612619318718669868907615189244643178364&
                &9604710976258237438709882933414201622978713022975908448898269286373397175572572843473932165513894328&
                &0948775427139028319745676136738066595818634893585336670047942640579585732561265895169788288873968502&
                &3576409450602037731566868670343358720132085137732083365284366309191514754276902232302595183222780229&
                &0333048746295176633732590892271134921001816237945675046107354701982076491636207145766942241484852876&
                &8086489045974168028871375519412956556734569711277377659269966781151202506367906250771838987905542729&
                &9602633739292957854646574862022345808897851021953319762924371865024136126553409742233338383148939476&
                &2688819583034676283720590690321437702466211493942477839239904785467381975953431360199181632139817087&
                &1675085388306946402985966654686374017597361994515490028221221197873309065711452256943483997268156661&
                &4251522601981091437164454815585766387477985848066653010274015762006422743531061320328958737883350699&
                &5010107953612418870689506323691217683866631324712647102533109103896945673048537987294807465898328021&
                &0616399905018381133672495392016893435858086953047467302012798815167555583471558849452903240275817359&
                &7412600246880120097460997601280569422971588083755852211710169314345911644907180640214262742781303093&
                &6362054083426905944815062311426831879988031400740033624376500891481190403873103717439643860930434152&
                &0077820977089318080209161337954445502789934971878811641353709185004519914977577688820318078268550811&
                &9598842219332616175613533636799875893648266143272209859623766305070288663132625316020772779359622743&
                &6981030373132763462837652106194804931955311030759723858460597094396859076238402414307660975253097132&
                &8698643152225068610430793799791351450546432634372714107713137332348308355668038997851202770977262560&
                &6860655592065973411067851629502656906251604510378685022379816969935014362348544555586522272088499721&
                &9946444206136083948321352132790842341486712676660929539344838800833207748229649079300799848460228620&
                &9317916049805688735829248332080887463530900166033204573480074339495592738215902398384614205016081150&
                &6669327808418122123950416238344051511059531020385411336197538432312088162779143008762730602999224040&
                &4412462631215227731882153726267258980995290409075434047558959478172735835465203332540303097147035552&
                &5666711990525067812075528567351636372299669796556262498468798624564731519949881412251505923493700744&
                &6335216604198320142967929437627737857666675125232132004943938027381035739733071077389290829983249430&
                &9929671156285733762483649064261848981602065505963476296523983015653616082982446966218901161161205188&
                &0573600170377567179171204447290536296387610124123253094661730508794512413417917215234868180985445947&
                &4633611169558495590616526594210716678969741229385520704603028673106854460886311076850901072143976966&
                &6670653340074110727042938673600505240289417987265120625262935190175503280434524070893838237104432038&
                &7277721187068571883463249210788857422002436533794328830424210014284256017507940795564055131778982162&
                &2057329034192487819427904384064457476669697518270041324739893645003100081320531996273644177857645536&
                &8545042222437577676482965113248928599178769221842229576106346149044548609079571934297226553085916258&
                &7592543596965131548109725448343837961730187194350398048494910675432603696794370092723457745633520895&
                &5119608085204333293851879810252603015301753358273689011732017729549391344820270439201073177077719806&
                &6340808266346052755156509338328217335355010644405352755527544829502078548861021691811911472766970454&
                &1126722373686640238868728507802237164806166737169811286535298575549796654582990432275142847727629502&
                &1305769671663557289499368071930128517595842219011940342211409668478145343257173098710205665337736566&
                &7695994299658695511962006986219821529127829161089064469294254157734218514131756969922934643982128786&
                &2715171334655164118356373428019349203374943169393056179361968313211330781317671346402999475798391627&
                &7899704387839522461273739469298912609371545163597105274885675371149316619814184131715268620939484405&
                &4032822037910079156926925384052248020095121565015480766860393059726428647308949687836979970427915464&
                &4264625542574505988870728965217856989288717655724378129280385306319189930760735776751678832851307548&
                &0329607523670401065187600417105444319087401626053234839268127494935061885924159151772184284570819457&
                &9765358561330329205363145261773965226880523298786386620674889009587176503669008414757444001413680605&
                &1851754109107612931858580108997222201353234246355066745929992414298433627776673565501154849803765982&
                &3801743520677023503489041175120829329692997134458297062148957929737209809196458870294106726869032085&
                &7789703094882329377767630717523835645468121709209222839288061729452480299859659067904004155322262027&
                &4942016841932467005817058434662245916974636886994084629484965489954062257150707733514203824510029893&
                &0740188138973309464198336206180093073990056649880937698690357135529391353848482852684893681528382991&
                &0410280771732290741176568547605485911343042882835702704191107389084055677618161337761939546366170150&
                &8064781253559946842752977101984760297902666892353040900170187885831791071277065381487511702153859041&
                &5299813558768597766083566649688989266040033065035240732139305422496656316421603624955865828341705144&
                &3447994139174194712038660873812088215000697427262799495041365132243338960326526617370362693352900229&
                &5839660406183978610341744635787604399627725957535437386160926670026801543280519042677441316699480894&
                &4875931667388354376924131257775463750780942146112953413750161644209045078267855172649330717659424934&
                &8335712735674451357884496678904100317012827438683814373992652581881866811580622571804449391481344574&
                &8721113352345494414054446470032427805567908094831174795204261577836387511406006137595003048602258126&
                &7574174240943346072781646716338402483610109902716905722880495175295252854374708163967596343005823957&
                &4178083920280304911846453277681629032012659221856535496262316447548683056149679053220966081343915763&
                &6618388485315900488605284740175857970500838688820831776374452618973739739617720957620005981443081902&
                &4300636730721404743123197614003206963487294419744356022978269747190299245009057165700504871383191949&
                &4899374942288264992976945103110345290533057003941954989124658125216431980606017233763887218072751504&
                &7096960229110614316970153571571481206941090011829947032962703662183625088631507092427471435324068470&
                &0366844465251076175358580515701393054153420428006834812855173272409462525200355138959896149898062525&
                &2210628999335118130478060363668307275831118339581618716511127649271497546196039918772873373199382754&
                &6560828617244201387568777027698045385338478763220413303633453325803060889236256471072252843526783119&
                &3479871927503164671562194351641764632839321378419898279406392257259948038445867866022404201010548148&
                &7253397872999082316105642775727365630729430694476221318450355038042498725633142324796624876140016499&
                &3630315406642793862426329905517326187048203464559341824317026461223979145994800153463344219940539205&
                &8817280808635659115943320567223211441185330248489643784516019287510770248820455510778539577940252666&
                &8183267612737208040264783872625984696777713170768969342562651487622257641705902318612115711079338922&
                &2493638262892467174711936785452484818671226836798093117387826527910160787492368254620398137406505131&
                &8731988042354812033108407558675436962949740512319483298252234589352170375247787814835721268045446829&
                &2317117377251138299913201780559797753723849597826002363700794792393814430199924797858736790730058164&
                &9952115153236795221485683501836480463808149826963803445797953052814946094155785057280525607436938816&
                &8429022866291042876643873792538883921257534909031812560345020981065950461122018135436055807910398208&
                &1484855520256575276221518623542883183408040492631138649359729684960991172376968732880996978620285511&
                &8920978864722624039207631304837709254566567316280789050587533720412634195873847049653211138198244266&
                &0719308767951120142177649511668883859734091231860247534722574046015100358565612400275756547152074564&
                &1633137898850424421600859605265441281300920250103584929436220562511251780317859133490449800210440206&
                &3981850043050466311875810414734973403058771563153667133211228742703665404508238629870510947427656995&
                &8638542779689576904473234941595273107383541712176187177722929884736371754403934807195232649597205636&
                &7221673933010790346005001099664009818203015805631886046528599114745693012019712138121354212324790087&
                &6231390929321586247720337913367593147267683928201056451758630035154869092151561740273050582062026461&
                &9572049643976530665071930187197763438414695944410131097851490700907933723704516209591343059105381927&
                &6905146017463471917031994260130109135398832398868757935821801928877154665144538645512469367057022196&
                &6054756561099772143655047379026794640167482518879370689566268763892362268011507829185486417838902115&
                &2013045682596657873774794142783874686585931915497399076756434719515907361068265152702994177444969608&
                &9592887399294629417678853384234804422154039327343881374143794048281132753541259730778406832518482134&
                &4637540401952767039481570552371988138457076195431968762699987960258703705461489278791473966013233379&
                &6757285178848543455309032854535650058524208397901319574386300684931853011117241480422198239039503450&
                &5655663330591727042233318955458942000042598046293770136501981539240756523705635887060941113869993907&
                &1751151952076710851450505936665537567847881544310871209165764039670118474872200059013387628379850978&
                &2307116366065133838722169400801537852265629436915335433281339392e9864_16
  logical, parameter :: test_ahuge2 = ahuge2 == zahuge2 .and. ahuge2 == dahuge2
  logical, parameter :: test_ahuge3 = ahuge3 == zahuge3 .and. ahuge3 == dahuge3
  logical, parameter :: test_ahuge4 = ahuge4 == zahuge4 .and. ahuge4 == dahuge4
  logical, parameter :: test_ahuge8 = ahuge8 == zahuge8 .and. ahuge8 == dahuge8
  logical, parameter :: test_ahuge10 = ahuge10 == zahuge10 .and. ahuge10 == dahuge10
  logical, parameter :: test_ahuge16 = ahuge16 == zahuge16 .and. ahuge16 == dahuge16

  real(2), parameter :: tiny2 = tiny(0._2), ztiny2 = real(z'0400', kind=2)
  real(3), parameter :: tiny3 = tiny(0._3), ztiny3 = real(z'0080', kind=3)
  real(4), parameter :: tiny4 = tiny(0._4), ztiny4 = real(z'00800000', kind=4)
  real(8), parameter :: tiny8 = tiny(0._8), ztiny8 = real(z'0010000000000000', kind=8)
  real(10), parameter :: tiny10 = tiny(0._10), ztiny10 = real(z'00018000000000000000', kind=10)
  real(16), parameter :: tiny16 = tiny(0._16), ztiny16 = real(z'00008000000000000000000000000000', kind=16)
  logical, parameter :: test_tiny2 = tiny2 == ztiny2
  logical, parameter :: test_tiny3 = tiny3 == ztiny3
  logical, parameter :: test_tiny4 = tiny4 == ztiny4
  logical, parameter :: test_tiny8 = tiny8 == ztiny8
  logical, parameter :: test_tiny10 = tiny10 == ztiny10
  logical, parameter :: test_tiny16 = tiny16 == ztiny16

  integer, parameter :: &
    max2 = maxexponent(0._2), &
    max3 = maxexponent(0._3), &
    max4 = maxexponent(0._4), &
    max8 = maxexponent(0._8), &
    max10 = maxexponent(0._10), &
    max16 = maxexponent(0._16)
  logical, parameter :: test_max2 = max2 == 15
  logical, parameter :: test_max3 = max3 == 127
  logical, parameter :: test_max4 = max4 == 127
  logical, parameter :: test_max8 = max8 == 1023
  logical, parameter :: test_max10 = max10 == 16383
  logical, parameter :: test_max16 = max16 == 32767

  integer, parameter :: &
    min2 = minexponent(0._2), &
    min3 = minexponent(0._3), &
    min4 = minexponent(0._4), &
    min8 = minexponent(0._8), &
    min10 = minexponent(0._10), &
    min16 = minexponent(0._16)
  logical, parameter :: test_min2 = min2 == -14
  logical, parameter :: test_min3 = min3 == -126
  logical, parameter :: test_min4 = min4 == -126
  logical, parameter :: test_min8 = min8 == -1022
  logical, parameter :: test_min10 = min10 == -16382
  logical, parameter :: test_min16 = min16 == -32766

  integer, parameter :: &
    irange1 = range(0_1), &
    irange2 = range(0_2), &
    irange4 = range(0_4), &
    irange8 = range(0_8), &
    irange16 = range(0_16)
  logical, parameter :: test_irange1 = irange1 == 2
  logical, parameter :: test_irange2 = irange2 == 4
  logical, parameter :: test_irange4 = irange4 == 9
  logical, parameter :: test_irange8 = irange8 == 18
  logical, parameter :: test_irange16 = irange16 == 38

  integer, parameter :: &
    arange2 = range(0._2), zrange2 = range((0._2,0._2)), &
    arange3 = range(0._3), zrange3 = range((0._3, 0._3)), &
    arange4 = range(0._4), zrange4 = range((0._4, 0._4)), &
    arange8 = range(0._8), zrange8 = range((0._8, 0._8)), &
    arange10 = range(0._10), zrange10 = range((0._10, 0._10)), &
    arange16 = range(0._16), zrange16 = range((0._16, 0._16))
  logical, parameter :: test_arange2 = arange2 == 4 .and. zrange2 == 4
  logical, parameter :: test_arange3 = arange3 == 37 .and. zrange3 == 37
  logical, parameter :: test_zrange4 = arange4 == 37 .and. zrange4 == 37
  logical, parameter :: test_zrange8 = arange8 == 307 .and. zrange8 == 307
  logical, parameter :: test_zrange10 = arange10 == 4931 .and. zrange10 == 4931
  logical, parameter :: test_zrange16 = arange16 == 9863 .and. zrange16 == 9863

end module
