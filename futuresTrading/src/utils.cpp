#include "../include/utils.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <numeric>
#include <windows.h>
#include <cmath>
#include <iomanip>        // put_time
#include <chrono>         // chrono::system_clock
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace std;

enum ExchangeType {
	SH = 0,
	SZ = 1
};


enum Side {
	Long = 0,
	Short = 1
};

string Utils::replaceString(const string& str, const string &src, const string &dest)
{
	string rtn = boost::replace_all_copy(str, src, dest);
	return rtn;
}


string Utils::getNextDay(const string& day) {
	string day1 = boost::replace_all_copy(day, "-", "");
	using namespace boost::gregorian;
	int yy = stringToInt(day1.substr(0, 4));
	int mm = stringToInt(day1.substr(4, 2));
	int dd = stringToInt(day1.substr(6));
	date d(yy, mm, dd);
	d += days(1);
	yy = d.year();
	mm = d.month();
	dd = d.day();
	stringstream ss;
	ss << yy;
	if (mm < 10)
	{
		ss << '0';
	}
	ss << mm;
	if (dd < 10)
	{
		ss << '0';
	}
	ss << dd;
	return ss.str();	
}

string Utils::getCurrentDateString() {
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	stringstream ss;
	ss << sTime.wYear;
	if (sTime.wMonth < 10)
	{
		ss << '0';
	}
	ss << sTime.wMonth;
	if (sTime.wDay < 10)
	{
		ss << '0';
	}
	ss << sTime.wDay;
	return ss.str();
}

string Utils::getCurrentTimeString() {
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	stringstream ss;
	if (sTime.wHour < 10)
	{
		ss << '0';
	}
	ss << sTime.wHour << ':';
		
	if (sTime.wMinute < 10)
	{
		ss << '0';
	}
	ss << sTime.wMinute << ':';

	if (sTime.wSecond < 10)
	{
		ss << '0';
	}
	ss << sTime.wSecond;
	return ss.str();
}



string Utils::entrustTimeFormat(const string& entrustTime) {
	
	if (entrustTime.length()<5)
	{
		string nullrtn;
		return nullrtn;
	}
	string ret;
	if (entrustTime.length() == 5)
	{
		ret += '0';
	}
	ret += entrustTime;
	ret.insert(4, ":");
	ret.insert(2, ":");
	
	return ret;
}

double Utils::getAvgTradeCost(const string& symbol, int nShares, double price, Side side) {
	double cost = 0.0;
	if (getExchangeType(symbol) == SH)
	{
		cost = nShares * price * 0.0003 + nShares * 0.0006;		
	}
	else
	{
		cost = nShares * price * 0.0003;
	}
	if (side == Short)
	{
		cost += nShares * price * 0.001;
	}
	return cost / nShares;
}


tm Utils::getCurrentTime() {
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());
	return *(std::localtime(&tt));
}


string Utils::exchangeOrWindCode2Exchange(const string& code) {
	vector<string> symbolAndMarket = Utils::split(code, '.');	
	if (symbolAndMarket.size() != 2) {
		return code;
	}
	else
	{
		return symbolAndMarket[0];
	}
}


double Utils::roundToHundredth(double val) {
	char valStr[100];
	sprintf(valStr, "%.2f", val);
	string str(valStr);
	return boost::lexical_cast<double>(str);
}


string Utils::exchangeOrWindCode2Wind(const string& code) {
	vector<string> symbolAndMarket = Utils::split(code, '.');
	if (symbolAndMarket.size() == 1) {
		return exchangeCode2Wind(code);
	}
	else
	{
		return boost::to_upper_copy(code);
	}
}

ExchangeType Utils::getExchangeType(const string& code) {
	string windCode = exchangeOrWindCode2Wind(code);
	if (windCode.length() < 2)
		return SH;
	string exchangeCode = windCode.substr(windCode.length() - 2, 2);
	if (exchangeCode == "SH")
		return SH;
	else
		return SZ;
}


string Utils::exchangeCode2Wind(const string& code) {
	string prefix = code.substr(0, 2);
	if (prefix == "60" || prefix == "90" || code[0] == '7' || code[0] == '5')
		return code + ".SH";
	else
		return code + ".SZ";
}

string Utils::windCode2Exchange(const string& code) {
	return code.substr(0, code.size() - 3);
}


int Utils::roundDown(int numberToRound, int multiple) {
	return ( (int) (numberToRound) / multiple) * multiple;
}


bool Utils::isCreditAccount(const string& account) {
	return account.substr(0, 3) == "800";
}

int Utils::roundUp(int numToRound, int multiple)
{
	if(multiple == 0)
	{
		return numToRound;
	}

	int roundDown = ( (int) (numToRound) / multiple) * multiple;
	int roundUp = roundDown + multiple; 
	int roundCalc = roundUp;
	return (roundCalc);
}

string Utils::concatenate(const vector<std::string>& v, const char* delims, bool tailDelim){
	std::stringstream ss;
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(ss, delims));
	std::string result = ss.str();
	if (!tailDelim)
	{
		result = result.substr(0, result.find_last_of(delims));
	}
	return result;
}

vector<std::string>& Utils::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<std::string> Utils::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	Utils::split(s, delim, elems);
	return elems;
}


string Utils::roundToHundredth(const std::string& str) {
	return str.substr(0, str.find('.')+3);
}

string Utils::roundStrToInteger(const std::string& str) {
	return str.substr(0, str.find('.'));
}


string Utils::getFormattedTime(const char* format) {
	std::time_t rawtime;
	std::tm* timeinfo;
	char buffer [80];
	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);
	std::strftime(buffer,80, format,timeinfo);
	return string(buffer);
}

void Utils::flatLogging(const string& str) {
	src::severity_logger< severity_level > lg;
	BOOST_LOG_SEV(lg, info) << str;
}

std::string Utils::doubleTostring(double data)
{
	ostringstream oss;
	oss << data;
	string str = oss.str();
	return str;
}

std::string Utils::precisionOut(double v, int precision)
{
	ostringstream oss;
	string str;
	oss << setiosflags(ios::fixed) << setprecision(precision) << v;
	str = oss.str();	
	//fixed为实数输出，若改为scientifitic则是科学技术法输出；setprecision(2)此处表示设置精度为2
	return str;
}

double Utils::roundDouble(double v, unsigned int precision) {
	int multiply = pow(10, precision);
	return (double)(roundToInt(v) * multiply) / multiply;
}


int Utils::roundToInt(double r) {
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}


string Utils::getDate(const char *date)
{
	string rtn = date;
	if (rtn.length() == 8)
	{
		rtn.insert(4, "-");
		rtn.insert(7, "-");
	}

	return rtn;
}

string Utils::getTime(const char *time)
{
	string rtn = time;
	if (rtn.length() == 6)
	{
		rtn.insert(2, ":");
		rtn.insert(5, ":");
	}

	return rtn;
}

string Utils::getFirstLetter(const char *strChs)
{
	static int li_SecPosValue[] = {
		1601, 1637, 1833, 2078, 2274, 2302, 2433, 2594, 2787, 3106, 3212,
		3472, 3635, 3722, 3730, 3858, 4027, 4086, 4390, 4558, 4684, 4925, 5249
	};
	static char* lc_FirstLetter[] = {
		"A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "O",
		"P", "Q", "R", "S", "T", "W", "X", "Y", "Z"
	};
	static char* ls_SecondSecTable =
		"CJWGNSPGCGNE[Y[BTYYZDXYKYGT[JNNJQMBSGZSCYJSYY[PGKBZGY[YWJKGKLJYWKPJQHY[W[DZLSGMRYPYWWCCKZNKYYGTTNJJNYKKZYTCJNMCYLQLYPYQFQRPZSLWBTGKJFYXJWZLTBNCXJJJJTXDTTSQZYCDXXHGCK[PHFFSS[YBGXLPPBYLL[HLXS[ZM[JHSOJNGHDZQYKLGJHSGQZHXQGKEZZWYSCSCJXYEYXADZPMDSSMZJZQJYZC[J[WQJBYZPXGZNZCPWHKXHQKMWFBPBYDTJZZKQHY"
		"LYGXFPTYJYYZPSZLFCHMQSHGMXXSXJ[[DCSBBQBEFSJYHXWGZKPYLQBGLDLCCTNMAYDDKSSNGYCSGXLYZAYBNPTSDKDYLHGYMYLCXPY[JNDQJWXQXFYYFJLEJPZRXCCQWQQSBNKYMGPLBMJRQCFLNYMYQMSQYRBCJTHZTQFRXQHXMJJCJLXQGJMSHZKBSWYEMYLTXFSYDSWLYCJQXSJNQBSCTYHBFTDCYZDJWYGHQFRXWCKQKXEBPTLPXJZSRMEBWHJLBJSLYYSMDXLCLQKXLHXJRZJMFQHXHWY"
		"WSBHTRXXGLHQHFNM[YKLDYXZPYLGG[MTCFPAJJZYLJTYANJGBJPLQGDZYQYAXBKYSECJSZNSLYZHSXLZCGHPXZHZNYTDSBCJKDLZAYFMYDLEBBGQYZKXGLDNDNYSKJSHDLYXBCGHXYPKDJMMZNGMMCLGWZSZXZJFZNMLZZTHCSYDBDLLSCDDNLKJYKJSYCJLKWHQASDKNHCSGANHDAASHTCPLCPQYBSDMPJLPZJOQLCDHJJYSPRCHN[NNLHLYYQYHWZPTCZGWWMZFFJQQQQYXACLBHKDJXDGMMY"
		"DJXZLLSYGXGKJRYWZWYCLZMSSJZLDBYD[FCXYHLXCHYZJQ[[QAGMNYXPFRKSSBJLYXYSYGLNSCMHZWWMNZJJLXXHCHSY[[TTXRYCYXBYHCSMXJSZNPWGPXXTAYBGAJCXLY[DCCWZOCWKCCSBNHCPDYZNFCYYTYCKXKYBSQKKYTQQXFCWCHCYKELZQBSQYJQCCLMTHSYWHMKTLKJLYCXWHEQQHTQH[PQ[QSCFYMNDMGBWHWLGSLLYSDLMLXPTHMJHWLJZYHZJXHTXJLHXRSWLWZJCBXMHZQXSDZP"
		"MGFCSGLSXYMJSHXPJXWMYQKSMYPLRTHBXFTPMHYXLCHLHLZYLXGSSSSTCLSLDCLRPBHZHXYYFHB[GDMYCNQQWLQHJJ[YWJZYEJJDHPBLQXTQKWHLCHQXAGTLXLJXMSL[HTZKZJECXJCJNMFBY[SFYWYBJZGNYSDZSQYRSLJPCLPWXSDWEJBJCBCNAYTWGMPAPCLYQPCLZXSBNMSGGFNZJJBZSFZYNDXHPLQKZCZWALSBCCJX[YZGWKYPSGXFZFCDKHJGXDLQFSGDSLQWZKXTMHSBGZMJZRGLYJB"
		"PMLMSXLZJQQHZYJCZYDJWBMYKLDDPMJEGXYHYLXHLQYQHKYCWCJMYYXNATJHYCCXZPCQLBZWWYTWBQCMLPMYRJCCCXFPZNZZLJPLXXYZTZLGDLDCKLYRZZGQTGJHHGJLJAXFGFJZSLCFDQZLCLGJDJCSNZLLJPJQDCCLCJXMYZFTSXGCGSBRZXJQQCTZHGYQTJQQLZXJYLYLBCYAMCSTYLPDJBYREGKLZYZHLYSZQLZNWCZCLLWJQJJJKDGJZOLBBZPPGLGHTGZXYGHZMYCNQSYCYHBHGXKAMTX"
		"YXNBSKYZZGJZLQJDFCJXDYGJQJJPMGWGJJJPKQSBGBMMCJSSCLPQPDXCDYYKY[CJDDYYGYWRHJRTGZNYQLDKLJSZZGZQZJGDYKSHPZMTLCPWNJAFYZDJCNMWESCYGLBTZCGMSSLLYXQSXSBSJSBBSGGHFJLYPMZJNLYYWDQSHZXTYYWHMZYHYWDBXBTLMSYYYFSXJC[DXXLHJHF[SXZQHFZMZCZTQCXZXRTTDJHNNYZQQMNQDMMG[YDXMJGDHCDYZBFFALLZTDLTFXMXQZDNGWQDBDCZJDXBZGS"
		"QQDDJCMBKZFFXMKDMDSYYSZCMLJDSYNSBRSKMKMPCKLGDBQTFZSWTFGGLYPLLJZHGJ[GYPZLTCSMCNBTJBQFKTHBYZGKPBBYMTDSSXTBNPDKLEYCJNYDDYKZDDHQHSDZSCTARLLTKZLGECLLKJLQJAQNBDKKGHPJTZQKSECSHALQFMMGJNLYJBBTMLYZXDCJPLDLPCQDHZYCBZSCZBZMSLJFLKRZJSNFRGJHXPDHYJYBZGDLQCSEZGXLBLGYXTWMABCHECMWYJYZLLJJYHLG[DJLSLYGKDZPZXJ"
		"YYZLWCXSZFGWYYDLYHCLJSCMBJHBLYZLYCBLYDPDQYSXQZBYTDKYXJY[CNRJMPDJGKLCLJBCTBJDDBBLBLCZQRPPXJCJLZCSHLTOLJNMDDDLNGKAQHQHJGYKHEZNMSHRP[QQJCHGMFPRXHJGDYCHGHLYRZQLCYQJNZSQTKQJYMSZSWLCFQQQXYFGGYPTQWLMCRNFKKFSYYLQBMQAMMMYXCTPSHCPTXXZZSMPHPSHMCLMLDQFYQXSZYYDYJZZHQPDSZGLSTJBCKBXYQZJSGPSXQZQZRQTBDKYXZK"
		"HHGFLBCSMDLDGDZDBLZYYCXNNCSYBZBFGLZZXSWMSCCMQNJQSBDQSJTXXMBLTXZCLZSHZCXRQJGJYLXZFJPHYMZQQYDFQJJLZZNZJCDGZYGCTXMZYSCTLKPHTXHTLBJXJLXSCDQXCBBTJFQZFSLTJBTKQBXXJJLJCHCZDBZJDCZJDCPRNPQCJPFCZLCLZXZDMXMPHJSGZGSZZQLYLWTJPFSYASMCJBTZKYCWMYTCSJJLJCQLWZMALBXYFBPNLSFHTGJWEJJXXGLLJSTGSHJQLZFKCGNNNSZFDEQ"
		"FHBSAQTGYLBXMMYGSZLDYDQMJJRGBJTKGDHGKBLQKBDMBYLXWCXYTTYBKMRTJZXQJBHLMHMJJZMQASLDCYXYQDLQCAFYWYXQHZ";
	string result;
	int H = 0;
	int L = 0;
	int W = 0;
	unsigned int stringlen = strlen(strChs);
	for (int i = 0; i < stringlen; i++) {
		H = (unsigned char)(strChs[i + 0]);
		L = (unsigned char)(strChs[i + 1]);
		if (H < 0xA1 || L < 0xA1) {
			result += strChs[i];
			continue;
		}
		else {
			W = (H - 160) * 100 + L - 160;
		}
		if (W > 1600 && W < 5590) {
			for (int j = 22; j >= 0; j--) {
				if (W >= li_SecPosValue[j]) {
					result += lc_FirstLetter[j];
					i++;
					break;
				}
			}
			continue;
		}
		else {
			i++;
			W = (H - 160 - 56) * 94 + L - 161;
			if (W >= 0 && W <= 3007)
				result += ls_SecondSecTable[W];
			else {
				result += (char)H;
				result += (char)L;
			}
		}
	}
	return result;
}


void Utils::boostCVSafeWait(boost::condition_variable& cv, boost::mutex::scoped_lock &lk, int seconds)
{
	try {
		cv.wait_for(lk, boost::chrono::seconds(seconds));
	}
	catch (exception& e) {

	}
	catch (boost::exception &e) {

	}
}