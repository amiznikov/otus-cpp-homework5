#include <iostream>
#include <limits>
#include <vector>

class IStatistics {
public:
	virtual ~IStatistics() {}

	virtual void update(double next) = 0;
	virtual double eval() const = 0;
	virtual const char * name() const = 0;
};

class Min : public IStatistics {
public:
	Min() : m_min{std::numeric_limits<double>::max()} {
	}

	void update(double next) override {
		if (next < m_min) {
			m_min = next;
		}
	}

	double eval() const override {
		return m_min;
	}

	const char * name() const override {
		return "min";
	}

private:
	double m_min;
};

class Max : public IStatistics {
public:
	Max() : m_max{std::numeric_limits<double>::min()} {
	}

	void update(double next) override {
		if (next > m_max) {
			m_max = next;
		}
	}

	double eval() const override {
		return m_max;
	}

	const char * name() const override {
		return "max";
	}

private:
	double m_max;
};

class Mean : public IStatistics {
public:
	Mean() = default;

	void update(double next) override {
		amount += next;
		numb++;
	}

	double eval() const override {
		return numb == 0 ? 0 : amount / numb;
	}

	const char * name() const override {
		return "mean";
	}

private:
	double amount{0};
	size_t numb{0};
};

class STD : public IStatistics {
public:
	STD(Mean& mean) : m_mean(mean) {}

	void update(double next) override {
		m_values.push_back(next);
	}

	double eval() const override {
		double sum = 0;
		double mean = m_mean.eval();
		for (const double& value : m_values) {

			sum += pow(abs(value - mean), 2);
		}
		return  m_values.size()  == 0 ? 0 : sum / m_values.size();
	}

	const char * name() const override {
		return "std";
	}
private:
	Mean& m_mean;
	std::vector<double> m_values;
};

class Percentile : public IStatistics {
public:
	Percentile(unsigned char percentile) : m_percentile{percentile} {
		m_name = "pct" + std::to_string(m_percentile);
	}
	void update(double next) override {
		m_values.push_back(next);
		//т.к в eval нельзя менять состояние объекта, то приходится костылить так

	}
	double eval() const override {
		std::sort(m_values.begin(), m_values.end());
		const double position = (m_values.size() - 1) * m_percentile / 100;
		const int lowerIndex = static_cast<int>(floor(position));
		const int upperIndex = static_cast<int>(ceil(position));
		const double fraction = position - lowerIndex;
		return m_values[lowerIndex] + fraction * (m_values[upperIndex] - m_values[lowerIndex]);
	}
	const char * name() const override {
		return m_name.c_str();
	}
private:
	unsigned char m_percentile{0};
	mutable std::vector<double> m_values;
	std::string m_name;
};

int main() {
	const size_t statistics_count = 6;
	IStatistics *statistics[statistics_count];

	statistics[0] = new Min{};
	statistics[1] = new Max{};
	statistics[2] = new Mean();
	statistics[3] = new STD(*dynamic_cast<Mean*>(statistics[2]));
	statistics[4] = new Percentile{90};
	statistics[5] = new Percentile{95};


	double val = 0;
	while (std::cin >> val) {
		for (size_t i = 0; i < statistics_count; ++i) {
			statistics[i]->update(val);
		}
	}

	// Handle invalid input data
	if (!std::cin.eof() && !std::cin.good()) {
		std::cerr << "Invalid input data\n";
		return 1;
	}

	// Print results if any
	for (size_t i = 0; i < statistics_count; ++i) {
		std::cout << statistics[i]->name() << " = " << statistics[i]->eval() << std::endl;
	}

	// Clear memory - delete all objects created by new
	for (size_t i = 0; i < statistics_count; ++i) {
		delete statistics[i];
	}

	return 0;
}