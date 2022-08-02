#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <optional>
#include <vector>

#include "CppRO.hpp"
#include "utility.hpp"

namespace CppRO::LinearProgramming {
template <typename Model>
void clearBranches(Model& _model) {
    return _model.clearBranches();
}

template <typename Model, typename Branch>
void applyBranch(Model& _model, Branch& _rng) {
    _model.applyBranch(_rng);
}

template <typename Model, typename Branch>
void removeBranch(Model& _model, Branch& _rng) {
    _model.removeBranch(_rng);
}

template <typename LinkingObject, typename Branch>
void removeFixedVariable(LinkingObject _linkingObj, const Branch& _branch) {
    _linkingObj.removeFixedVariable(_branch);
}

template <typename LinkingObject, typename Branch>
void addFixedVariable(LinkingObject _linkingObj, const Branch& _branch) {
    _linkingObj.addFixedVariable(_branch);
}

template <typename LinkingObject>
void clear(LinkingObject _linkingObj) {
    _linkingObj.clear();
}

struct BestBoundExplorationPolicy;
struct DeepestBestBoundExplorationPolicy;
struct HighestBestBoundExplorationPolicy;

template <typename ContaineeType>
class BABTree {
  public:
    struct BABNode {
        BABNode(int _id, double _parentLB, BABTree* _tree,
            const ContaineeType& _val, int _parent = -1)
            : id(_id)
            , parent(_parent)
            , parentLB(_parentLB)
            , obj(_val)
            , tree(_tree)
            , depth(_parent != -1 ? (getParent().depth + 1) : 0) {}

        ~BABNode() = default;
        BABNode(const BABNode&) = default;
        BABNode& operator=(const BABNode&) = default;
        BABNode(BABNode&&) noexcept = default;
        BABNode& operator=(BABNode&&) noexcept = default;

        const BABNode& getParent() const {
            CppRO_ASSERT(parent != -1);
            CppRO_ASSERT(parent < tree->size());
            return tree->nodes[parent];
        }

        BABNode& getParent() {
            CppRO_ASSERT(parent != -1);
            CppRO_ASSERT(parent < tree->size());
            return tree->nodes[parent];
        }

        [[nodiscard]] bool hasParent() const { return parent != -1; }

        void kill() {
            CppRO_ASSERT(visited);
            closed = true;
        }

        /**
         * This function is called when we close it
         * If all brother are closed, we close the parent
         */
        void visit() {
            visited = true;
            if (hasParent() && --getParent().nbOpenChildren == 0) {
                getParent().kill();
            }
        }

        void addChild(const ContaineeType& _obj, double _LB) {
            CppRO_ASSERT(!epsilon_less<double>()(_LB, parentLB));
            nbOpenChildren++;
            tree->push_back(_obj, _LB, id);
        }

        int id;
        int parent;
        double parentLB;
        ContaineeType obj;
        BABTree* tree;
        int depth;

        int nbOpenChildren = 0;
        bool closed = false;
        bool visited = false;

    }; // struct BABNode

    class Node {
      public:
        explicit Node(BABNode& _node)
            : m_tree(_node.tree)
            , m_pos(_node.id) {}

        Node(BABTree* _tree, int _pos)
            : m_tree(_tree)
            , m_pos(_pos) {}

        ~Node() = default;

        Node(const Node&) = default;
        Node& operator=(const Node&) = default;
        Node(Node&&) noexcept = default;
        Node& operator=(Node&&) noexcept = default;

        const BABNode* operator->() const { return &m_tree->nodes[m_pos]; }

        BABNode* operator->() { return &m_tree->nodes[m_pos]; }

      private:
        BABTree* m_tree;
        int m_pos;
    };

    BABTree() = default;
    ~BABTree() = default;

    BABTree(const BABTree&) = default;
    BABTree& operator=(const BABTree&) = default;
    BABTree(BABTree&&) noexcept = default;
    BABTree& operator=(BABTree&&) noexcept = default;

    void push_back(
        const ContaineeType& _obj, double _parentLB, int _parent = -1) {
        nodes.emplace_back(nodes.size(), _parentLB, this, _obj, _parent);
    }

    Node back() const {
        CppRO_ASSERT(!empty());
        return {this, size() - 1};
    }

    Node back() {
        CppRO_ASSERT(!empty());
        return {this, size() - 1};
    }

    [[nodiscard]] int size() const { return nodes.size(); }

    [[nodiscard]] int getNbOpenNodes() const {
        return std::count_if(nodes.begin(), nodes.end(),
            [](auto&& _node) { return !_node.visited; });
    }

    [[nodiscard]] double getBestBound() const {
        const auto isValid = [](auto&& _n) { return !_n.closed; };
        const auto comp = [](auto&& _n1, auto&& _n2) {
            isValid(_n1) && _n1.lowerBound < _n2.lowerBound;
        };
        const auto ite = std::find_if(nodes.begin(), nodes.end(), isValid);
        CppRO_ASSERT(ite != nodes.end());
        return std::min_element(ite, nodes.end(), comp)->lowerBound;
    }

    [[nodiscard]] bool empty() const { return nodes.empty(); }

    void pop_back() {
        CppRO_ASSERT(!empty());
        nodes.pop_back();
    }

    template <typename ExplorationPolicy>
    Node getBestNode(ExplorationPolicy&& _explorationPolicy) {
        const auto firstValidNodeIte = std::find_if(nodes.begin(), nodes.end(),
            [](auto&& _node) { return !_node.visited; });
        CppRO_ASSERT(firstValidNodeIte != nodes.end());

        const auto minIte = std::min_element(
            firstValidNodeIte, nodes.end(), [&](auto&& _node1, auto&& _node2) {
                return !_node1.visited
                       && _explorationPolicy.compareNode(_node1, _node2);
            });

        const auto bestId = std::distance(nodes.begin(), minIte);
        // std::cout << "Best node has id: " << bestId << '\n';
        CppRO_ASSERT(!nodes[bestId].visited);
        return {this, bestId};
    }

  private:
    std::vector<BABNode> nodes;
};

struct BestBoundExplorationPolicy {
    template <typename Node>
    bool compareNode(const Node& _node1, const Node& _node2) {
        return !epsilon_less<double>()(_node2.parentLB, _node1.parentLB);
    }
};

struct DeepestBestBoundExplorationPolicy {
    template <typename Node>
    bool compareNode(const Node& _node1, const Node& _node2) {
        return _node1.depth > _node2.depth
               || (_node1.depth == _node2.depth
                   && !epsilon_less<double>()(
                       _node2.parentLB, _node1.parentLB));
    }
};

struct HighestBestBoundExplorationPolicy {
    template <typename Node>
    bool compareNode(const Node& _node1, const Node& _node2) {
        return _node1.depth < _node2.depth
               || (_node1.depth == _node2.depth
                   && !epsilon_less<double>()(
                       _node2.parentLB, _node1.parentLB));
    }
};

template <typename RMP, typename PricingProblem, typename = void>
struct IsPricingProblem : std::false_type {};

template <typename RMP, typename PricingProblem>
struct IsPricingProblem<RMP, PricingProblem,
    std::enable_if_t<
        std::is_same_v<typename PricingProblem::Column, typename RMP::Column>>>
    : std::true_type {};

/*template<typename RMP, typename... PricingProblems, typename = void>
struct IsRMPSolveFunction : std::false_type
{};

template<typename RMP, typename... PricingProblems>
struct IsPricingProblem<RMP, PricingProblems,
    std::is_invocable_v<SolveFunction, bool(RMP&, typename
ThreadingBase<PricingProblems...>&, bool)>> : std::true_type
{};*/

struct BaseBranchAndPriceParameters {
    int nbThreads = 1;
    bool verbose = true;
    std::chrono::duration<long> timeout = std::chrono::minutes(10);
    double gapEps = 1e-4;
};

template <typename BranchType>
struct BranchAndPriceParameters : public BaseBranchAndPriceParameters {
    explicit BranchAndPriceParameters(const BaseBranchAndPriceParameters& _base)
        : BaseBranchAndPriceParameters(_base) {}
    using Branch = BranchType;
};

template <typename RMP, typename SolveFunction, typename UpperBoundFunction,
    typename ExplorationPolicy, typename BranchingFunction>
auto branchAndPriceOnMaster(RMP& _rmp, SolveFunction _solveFunction,
    UpperBoundFunction _upperFunction, BranchingFunction _branchingFunc,
    const BaseBranchAndPriceParameters& _params =
        BaseBranchAndPriceParameters(),
    ExplorationPolicy _explorationPolicy = ExplorationPolicy()) ->
    typename std::invoke_result_t<UpperBoundFunction, RMP> {

    const auto getElapsedTime = [start = std::chrono::system_clock::now()]() {
        return std::chrono::duration<double>(
            std::chrono::system_clock::now() - start);
    };
    using Branch =
        typename std::invoke_result_t<BranchingFunction, RMP>::value_type;
    BABTree<Branch> tree;
    double upperBound = std::numeric_limits<double>::max();
    double lowerBound = -std::numeric_limits<double>::max();

    using SolutionType = std::invoke_result_t<UpperBoundFunction, RMP>;
    SolutionType bestSolution = std::nullopt;
    if (getElapsedTime() > _params.timeout) {
        return bestSolution;
    }
    if (_solveFunction(_rmp, true)) {
        if (_rmp.isIntegral()) {
            std::cout << "Root is integral\n";
            return _rmp.getSolution();
        } else {
            std::cout << "Root is not integral...." << std::flush;
            if (const auto& branches = _branchingFunc(_rmp);
                !branches.empty()) {
                for (const auto& branch : branches) {
                    tree.push_back(branch, _rmp.getObjValue());
                }
            } else {
                std::cout << "Could not create branches..." << std::flush;
                return bestSolution;
            }

            // Try to get UB...
            if (auto tmpSol = _upperFunction(_rmp);
                tmpSol.has_value()
                && (!bestSolution
                    || bestSolution->getObjValue() > tmpSol->getObjValue())) {
                bestSolution = std::move(*tmpSol);
                bestSolution->setLowerBound(lowerBound);
                upperBound = bestSolution->getObjValue();
                std::cout << "Found improving solution with value="
                          << bestSolution->getObjValue() << '\n';
            } else {
                std::cout << "No new UB found...";
            }
        }
    }
    int previousNodeId = -1;
    while (tree.getNbOpenNodes() > 0) {
        // Logging
        lowerBound = tree.getBestBound();
        if (_params.verbose) {
            std::cout << "# Nodes: " << tree.getNbOpenNodes() << " / "
                      << tree.size() << ", time: "
                      << std::chrono::duration_cast<std::chrono::seconds>(
                             getElapsedTime())
                             .count()
                      << "s, LB: " << lowerBound;
            if (bestSolution) {
                std::cout << ", UB: " << upperBound << ", epsilon: "
                          << (upperBound - lowerBound) / lowerBound;
            }
            std::cout << "..." << std::flush;
        }
        if (bestSolution) {
            bestSolution->setLowerBound(
                std::min(bestSolution->getObjValue(), lowerBound));
        }
        if (epsilon_less_equal<double>()(
                (upperBound - lowerBound) / lowerBound, _params.gapEps)) {
            if (_params.verbose) {
                std::cout << "Epsilon gap reached" << std::flush;
            }
            return bestSolution;
        }

        if (getElapsedTime() > _params.timeout) {
            if (_params.verbose) {
                std::cout << "Timeout reached" << std::flush;
            }
            return bestSolution;
        }
        // Get best node not visited
        auto currentLeaf = tree.getBestNode(_explorationPolicy);
        std::cout << "Current node: " << currentLeaf->id
                  << ", Depth: " << currentLeaf->depth << "...";
        currentLeaf->visit();

        CppRO_ASSERT([&]() {
            if (epsilon_less_equal<double>()(
                    lowerBound, currentLeaf->parentLB)) {
                return true;
            }
            return false;
        }());
        std::cout << "..." << std::flush;

        if (previousNodeId != currentLeaf->parent) {
            // Remove all constraints to master
            clearBranches(_rmp);
            // Add all new constraints to master
            auto leaf = currentLeaf;
            while (leaf->hasParent()) {
                _rmp.add(leaf->obj);
                leaf = leaf->getParent();
            }
            _rmp.add(leaf->obj);
        } else {
            _rmp.add(currentLeaf->obj);
        }
        previousNodeId = currentLeaf->id;

        if (_solveFunction(_rmp, false)) {
            std::cout << "Current solution (" << _rmp.getObjValue() << ") ";
            // Kill node if lower bound is worse than best upper bound
            if (epsilon_less<double>()(upperBound, _rmp.getObjValue())) {
                std::cout << "is worse than upper bound...";
                currentLeaf->kill();
            } else if (_rmp.isIntegral()) {
                std::cout << "is integral..." << std::flush;

                // Select solution if integral and better than upper bound
                if (epsilon_less<double>()(_rmp.getObjValue(), upperBound)) {
                    std::cout << "and improving..." << std::flush;
                    upperBound = _rmp.getObjValue();
                    bestSolution = _rmp.getSolution();
                    bestSolution->setObjValue(upperBound);
                }

                // Stop if optimal
                if (epsilon_equal<double>()(upperBound, lowerBound)) {
                    break;
                }
                currentLeaf->kill();
            } else {
                // If solution not integral, get new branching decisions
                std::cout << "is not integral...." << std::flush;
                if (const auto& branches = _branchingFunc(_rmp);
                    !branches.empty()) {
                    for (const auto& branch : branches) {
                        currentLeaf->addChild(branch, _rmp.getObjValue());
                    }
                } else {
                    std::cout << "Could not create branches..." << std::flush;
                    currentLeaf->kill();
                }

                // Try to get UB...
                if (auto tmpSol = _upperFunction(_rmp);
                    tmpSol.has_value()
                    && (!bestSolution
                        || bestSolution->getObjValue()
                               > tmpSol->getObjValue())) {
                    bestSolution = std::move(*tmpSol);
                    bestSolution->setLowerBound(lowerBound);
                    upperBound = bestSolution->getObjValue();
                    std::cout << "Found improving solution with value="
                              << bestSolution->getObjValue() << '\n';
                } else {
                    std::cout << "No new UB found...";
                }
            }
        } else {
            std::cout << "Solution is not feasible..." << std::flush;
            currentLeaf->kill();
        }
        std::cout << '\n';
    }
    if (_params.verbose) {
        std::cout << "Whole tree explored" << std::flush;
    }

    // Make sure that we have the correct lower bound
    if (tree.getNbOpenNodes() == 0) {
        bestSolution->setLowerBound(bestSolution->getObjValue());
    } else {
        bestSolution->setLowerBound(lowerBound);
    }

    return bestSolution;
}
} // namespace CppRO::LinearProgramming
#endif // BRANCH_AND_BOUND_HPP
