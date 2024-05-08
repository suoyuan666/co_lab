#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using reslist = std::vector<std::int16_t>;

constexpr std::int16_t RES_SIZE = 3;
constexpr std::int16_t P_SZIE = 5;

class Process {
 private:
  reslist max;
  reslist need;
  reslist allocation;
  reslist request;
  bool finish{false};

 public:
  static std::unique_ptr<const reslist> total;
  static std::shared_ptr<reslist> available;

  Process()
      : allocation(reslist(RES_SIZE)),
        max(reslist(RES_SIZE)),
        request(reslist(RES_SIZE)),
        need(reslist(RES_SIZE)) {}

  Process(reslist &&max, reslist &&allocation)
      : allocation(std::move(allocation)),
        max(std::move(max)),
        request(reslist(RES_SIZE)) {
    reslist tmp = reslist(RES_SIZE);
    std::transform(
        this->max.begin(), this->max.end(), this->allocation.begin(),
        tmp.begin(),
        [](std::int16_t a, std::int16_t b) -> std::int16_t { return a - b; });
    this->need = std::move(tmp);
  }

  auto reslist2str(const reslist *val) const -> std::string {
    std::string str;
    for (auto val : *val) {
      str += std::to_string(val);
      str += ' ';
    }
    return str;
  }

  auto set_request(reslist &&value) -> void {
    this->request = std::move(value);
  }
  auto set_request(std::int16_t value, std::int16_t index) -> void {
    if (request.size() > index) {
      request.at(index) = request.at(index) * 10 + value;
    } else {
      request[index] = value;
    }
  }

  auto get_need() const -> reslist { return this->need; }
  auto get_max() const -> reslist { return this->max; }
  auto get_allocation() const -> reslist { return this->allocation; }
  auto get_request() const -> reslist { return this->request; }

  auto get_need2str() const -> std::string { return reslist2str(&need); }
  auto get_max2str() const -> std::string { return reslist2str(&max); }
  auto get_allocation2str() const -> std::string {
    return reslist2str(&allocation);
  }
  auto get_request2str() const -> std::string { return reslist2str(&request); }
};

reslist tmp{10, 5, 7};
std::unique_ptr<const reslist> Process::total =
    std::make_unique<reslist>(std::move(tmp));
std::shared_ptr<reslist> Process::available = std::make_shared<reslist>(RES_SIZE);

bool ck4sec(Process pslist[P_SZIE], std::int16_t index);
void ck4sec_core(Process pslist[],
                 std::shared_ptr<std::vector<std::int16_t>> work,
                 std::vector<bool> &finish,
                 std::vector<std::int16_t> &safeSequence);

int main(void) {
  Process pslist[P_SZIE]{{{7, 5, 3}, {0, 1, 0}},
                         {{3, 2, 2}, {2, 0, 0}},
                         {{9, 0, 2}, {3, 0, 2}},
                         {{2, 2, 2}, {2, 1, 1}},
                         {{4, 3, 3}, {0, 0, 2}}};

  std::cout << "当前各个进程的需求和占有情况:\n";
  std::cout << "    work    need    alloc" << std::endl;
  for (std::uint16_t i = 0; i < P_SZIE; ++i) {
    std::cout << "P" << i << "  " << pslist[i].get_max2str() << "  "
              << pslist[i].get_need2str() << "  "
              << pslist[i].get_allocation2str() << '\n';
  }

  {
    reslist tmp(RES_SIZE);
    for (std::int16_t i = 0; i < RES_SIZE; ++i) {
      for (auto &ps : pslist) {
        tmp.at(i) += ps.get_allocation().at(i);
      }
    }
    std::transform(Process::total->begin(), Process::total->end(), tmp.begin(),
                   Process::available->begin(),
                   [](int16_t a, int16_t b) -> int16_t { return a - b; });
  }

  std::cout << "请输入当前要请求资源的进程编号:" << std::endl;
  std::int16_t pid{0};
  {
    std::string tmp;
    char c;
    std::getline(std::cin, tmp);
    std::istringstream iss{tmp};

    if (!(iss >> pid)) {
      throw std::invalid_argument("错误的输入");
    }
  }

  std::cout << "现在输入该进程的请求序列(例: 1 2 3)" << std::endl;

  {
    std::int16_t req;
    std::string tmp;
    char c;

    std::getline(std::cin, tmp);
    std::istringstream iss{tmp};

    for (std::int16_t i = 0; i < RES_SIZE; ++i) {
      if (iss >> req) {
        pslist[pid].set_request(req, i);
      } else {
        throw std::invalid_argument("错误的输入");
      }
    }
  }

  std::cout << "可能的安全序列情况:" << std::endl;

  if (ck4sec(pslist, pid) == false) {
    std::cerr << "无安全序列" << std::endl;
  }
  return 0;
}

bool ck4sec(Process pslist[P_SZIE], std::int16_t index) {
  reslist alloc_tmp(3);
  reslist avlble_tmp(3);
  std::vector<std::int16_t> pidlist;

  for (std::int16_t i = 0; i < RES_SIZE; ++i) {
    if (pslist[index].get_request().at(i) > pslist[index].get_need().at(i)) {
      std::cerr << "请求的" << i << "号资源数量超过当前进程总共所需要的资源"
                << std::endl;
      return false;
    }
    if (pslist[index].get_request().at(i) > Process::available->at(i)) {
      std::cerr << "请求的" << i << "号资源数量超过当前系统空闲的资源"
                << std::endl;
      return false;
    }
  }
  std::vector<std::int16_t> safeSequence;
  std::vector<bool> finish(P_SZIE, false);

  for (int i = 0; i < RES_SIZE; i++) {
    Process::available->at(i) -= pslist[index].get_request().at(i);
    pslist[index].get_allocation().at(i) += pslist[index].get_request().at(i);
  }

  auto work = Process::available;
  ck4sec_core(pslist, work, finish, safeSequence);

  for (int i = 0; i < RES_SIZE; i++) {
    Process::available->at(i) += pslist[index].get_request().at(i);
    pslist[index].get_allocation().at(i) -= pslist[index].get_request().at(i);
  }
  return true;
}

void ck4sec_core(Process pslist[],
                 std::shared_ptr<std::vector<std::int16_t>> work,
                 std::vector<bool> &finish,
                 std::vector<std::int16_t> &safeSequence) {
  bool allFinished = true;
  for (int i = 0; i < P_SZIE; i++) {
    if (!finish[i]) {
      bool canAllocate = true;
      for (int j = 0; j < RES_SIZE; j++) {
        if (pslist[i].get_need().at(j) > work->at(j)) {
          canAllocate = false;
          break;
        }
      }
      if (canAllocate) {
        allFinished = false;
        finish[i] = true;
        safeSequence.push_back(i);
        for (int j = 0; j < RES_SIZE; j++) {
          work->at(j) += pslist[i].get_allocation().at(j);
        }
        ck4sec_core(pslist, work, finish, safeSequence);
        finish[i] = false;
        safeSequence.pop_back();
        for (int j = 0; j < RES_SIZE; j++) {
          work->at(j) -= pslist[i].get_allocation().at(j);
        }
      }
    }
  }
  if (allFinished) {
    for (auto i : safeSequence) {
      if (i == *(safeSequence.end() - 1)) {
        std::cout << "P" << i << "\n";
        break;
      }
      std::cout << "P" << i << " -> ";
    }
  }
}