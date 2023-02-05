#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <ranges>
using namespace std;

bool is_good_anonimity(uint anonimity, uint size) {
	uint min_anonimity;
	if (size < 51000)
		min_anonimity = 10;
	else if (size < 105000)
		min_anonimity = 7;
	else
		min_anonimity = 5;
	return anonimity >= min_anonimity;
}

struct values {
	map<string, uint8_t> data;
	auto get_id(string value) {
		if (data.contains(value))
			return data[value];
		else
			return data[value] = data.size();
	}
	auto sorted() {
		typedef pair<string_view, uint8_t> T;
		const auto buffer = new T[data.size()];
		ranges::copy(data, buffer);
		ranges::sort(buffer, buffer+data.size(), {}, &T::second);
		return buffer;
	}
};

string to_area(string& str) {
	const auto key1 = (const char*)u8"Ленинградская область";
	if (str.find(key1) != string::npos)
		return key1;
	
	const auto key2 = (const char*)u8"район";
	auto pos = str.find(key2, 0);
	if (pos == string::npos)
		return (const char*)u8"г. Санкт-Петербург";

	auto ptr = str.c_str() + pos;
	const auto end = ptr + strlen(key2);
	while (--ptr > str.c_str())
		if (*ptr == ',') {
			ptr++;
			while (*ptr == ' ')
				ptr++;
			break;
		}

	return string(ptr, end);
}

struct person {
	uint8_t town_id;
	uint8_t job_id;
	uint salary;
	strong_ordering operator<=>(const person& b) const = default;
	bool operator==(const person& b) const = default;
};


int main() {
	ifstream input("input.csv", ios::binary);
	vector<person> database;
	values regions, jobs;

	cout <<
		"Count k-anonymity - 0\n"
		"Anonymize dataset - 1\n";
	uint mode;
	cin >> mode;
	if (mode > 1) return 5;

	uint min_anonymity = 1;
	if (mode == 1) {
		cout << "minimum anonymity = ";
		cin >> min_anonymity;
	}

	string buffer;
	for (person p; !input.eof(); database.push_back(p)) {
		input.ignore(numeric_limits<streamsize>::max(), ';');
		if (input.eof()) break;

		input.ignore(numeric_limits<streamsize>::max(), ';');

		input.ignore(1);
		getline(input, buffer, '"');
		input.ignore(1);
		buffer = to_area(buffer);
		p.town_id = regions.get_id(buffer);

		getline(input, buffer, ';');
		p.job_id = jobs.get_id(buffer);

		getline(input, buffer);
		p.salary = stoi(buffer);
		p.salary -= p.salary % 20000;
	}

	ofstream output("output.csv", ios::binary);
	ranges::sort(database);
	uint equal_num = 1, k_anonymity = 0xffffffff;
	database.reserve(database.size() + 1);
	*database.end() = { 0,0,0 };
	auto regions_data = regions.sorted();
	auto jobs_data = jobs.sorted();
	for (auto& person : database) {
		if (person == (&person)[1])
			equal_num++;
		else {
			if (equal_num >= min_anonymity) {
				for (auto i = equal_num; i; i--) {
					output << "*;*;";
					output << regions_data[person.town_id].first << ';';
					output << jobs_data[person.job_id].first << ';';
					output << person.salary << '-' << person.salary+19999 << '\n';
				}
				k_anonymity = min(k_anonymity, equal_num);
			}
			equal_num = 1;
		}
	}

	if (k_anonymity == 0xffffffff)
		cout << "Result database is empty!";
	else
		cout << "k_anonimity = " << k_anonymity << '\n';
	delete[] jobs_data;

	if (mode == 0) remove("output.csv");
	return 0;
}
