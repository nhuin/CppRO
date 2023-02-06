#ifndef CPPRO_CONSTRAINEDSHORTESTPATH
#define CPPRO_CONSTRAINEDSHORTESTPATH

#include <CppRO/cplex_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/range.hpp>
#include <boost/range/combine.hpp>
#include <vector>

namespace CppRO {

class CompactConstrainedShortestPathModel {
    IloModel m_model;
    // Vars
    std::vector<IloNumVar> m_flowVars;

    std::vector<IloRange> m_flowConservationConstraints;
    IloRange m_delayConstraint;

    std::size_t m_currentSource{0};
    std::size_t m_currentTarget{0};

  public:
    /**
     * Build a model according to the graph and the delay of the link
     *
     * \tparam Graph A class representing a graph
     * \tparam DelayPropertyMap A class representing a property map of delays
     **/
    template <typename Graph>
    CompactConstrainedShortestPathModel(IloEnv& _env, const Graph& _graph);

    /**
     * Set the source and destination of the constrained shortest path
     */
    void setNodePair(std::size_t _source, std::size_t _target) {
        m_flowConservationConstraints[m_currentTarget].setBounds(0.0, 0.0);
        m_flowConservationConstraints[m_currentSource].setBounds(0.0, 0.0);
        m_currentSource = _source;
        m_currentTarget = _target;
        m_flowConservationConstraints[m_currentTarget].setBounds(-1.0, -1.0);
        m_flowConservationConstraints[m_currentSource].setBounds(1.0, 1.0);
    }

    template <typename DelayRange>
    void setDelays(const DelayRange& _delayRange);

    /**
     * Returns the built model
     */
    [[nodiscard]] const IloModel& getModel() const { return m_model; }

    /**
     * Return the flow variables
     */
    [[nodiscard]] const std::vector<IloNumVar>& getFlowVars() const {
        return m_flowVars;
    }

    void setMaxDelay(const double _maxDelay) {
        m_delayConstraint.setUB(_maxDelay);
    }
};

template <typename Graph>
CompactConstrainedShortestPathModel::CompactConstrainedShortestPathModel(
    IloEnv& _env, const Graph& _graph)
    : m_model(_env)
    , m_flowVars([&] {
        std::vector<IloNumVar> retval;
        for (std::size_t i = 0; i < num_edges(_graph); ++i) {
            retval.emplace_back(IloAdd(m_model,
                IloNumVar(m_model.getEnv(), 0, 1, IloNumVar::Type::Bool,
                    std::string("f_" + std::to_string(i)).c_str())));
        }
        return retval;
    }())
    , m_flowConservationConstraints([&] {
        std::vector<IloRange> retval;
        for (const auto u : boost::make_iterator_range(vertices(_graph))) {
            IloExpr expr(m_model.getEnv());
            for (const auto ed :
                boost::make_iterator_range(out_edges(u, _graph))) {
                expr += m_flowVars[get(boost::edge_index, _graph, ed)];
            }
            for (const auto ed :
                boost::make_iterator_range(in_edges(u, _graph))) {
                expr -= m_flowVars[get(boost::edge_index, _graph, ed)];
            }
            retval.emplace_back(IloAdd(m_model, IloRange(expr == 0)));
            expr.end();
        }
        return retval;
    }())
    , m_delayConstraint(
          IloAdd(m_model, IloRange(m_model.getEnv(), -IloInfinity, 0.0)))

{}

template <typename DelayRange>
void CompactConstrainedShortestPathModel::setDelays(
    const DelayRange& _delayRange) {
    IloExpr arcSumExpr(m_model.getEnv());
    for (const auto& [delay, var] : boost::combine(_delayRange, m_flowVars)) {
        arcSumExpr += static_cast<IloNum>(delay)
                      * var; // cast to IloNum to avoid ambiguous overload
    }
    m_delayConstraint.setExpr(arcSumExpr);
}

} // namespace CppRO
#endif
