#include <boost/filesystem.hpp>
#include <boost/random.hpp>
#include <chrono>
#include <iostream>
#include <ismrmrd/dataset.h>
#include <ismrmrd/ismrmrd.h>

using namespace ISMRMRD;

static boost::random::mt19937 rng;
static boost::random::uniform_real_distribution<float> dist = boost::random::uniform_real_distribution<float>();

float create_random_float() {
    return dist(rng);
}

int main(int argc, char **argv) {

    boost::filesystem::path temp = boost::filesystem::unique_path();

    Acquisition acq = Acquisition(256, 32, 2);

    std::vector<Acquisition> acqs(10240, acq);
    for (size_t i = 0; i < acqs.size(); i++) {
        std::generate((float *)acqs[i].data_begin(), (float *)acqs[i].data_end(), create_random_float);
        std::generate((float *)acqs[i].traj_begin(), (float *)acqs[i].traj_end(), create_random_float);
    }

    {

        auto start = std::chrono::high_resolution_clock::now();
        Dataset dataset = Dataset(temp.string().c_str(), "/test", true);

        std::for_each(acqs.begin(),acqs.end(),[&dataset](Acquisition& acq){ dataset.appendAcquisition(acq);});

        auto duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "Write duration: " << duration.count() << "s" << std::endl;
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        Dataset dataset = Dataset(temp.string().c_str(), "/test", false);
        for (size_t i = 0; i < acqs.size(); i++) {
            Acquisition acq;
            dataset.readAcquisition(uint32_t(i), acq);
        }
        auto duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "Read duration: " << duration.count() << "s" << std::endl;
    }

    boost::filesystem::remove(temp);
}