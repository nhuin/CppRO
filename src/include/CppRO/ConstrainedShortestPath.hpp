#ifndef CPPRO_CONSTRAINEDSHORTESTPATH
#define CPPRO_CONSTRAINEDSHORTESTPATH

#include <CppRO/cplex_utility.hpp>
#include <boost/range.hpp>
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
    template <typename Graph, typename DelayPropertyMap>
    CompactConstrainedShortestPathModel(IloEnv& _env, const Graph& _graph);

    /**
     * Set the source and destination of the constrained shortest path
     */
    void setNodePair(std::size_t _source, std::size_t _target);

    template <typename DelayPropertyMap>
    void setDelays(const DelayPropertyMap& _delayMap);

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

template <typename Graph, typename DelayPropertyMap>
CompactConstrainedShortestPathModel::CompactConstrainedShortestPathModel(
    IloEnv& _env, const Graph& _graph)
    : m_model(_env)
    , m_flowVars(num_edges(_graph),
          IloAdd(m_model, IloNumVar(m_model.getEnv(), 0, 1, ILOBOOL)))
    , m_flowConservationConstraints([&] {
        std::vector<IloRange> retval;
        for (const auto u : boost::make_iterator_range(vertices(_graph))) {
            IloExpr expr(m_model.getEnv());
            for (const auto ed :
                boost::make_iterator_range(out_edges(u, _graph))) {
                expr += m_flowVars[get(boost::edge_index_t, ed, _graph)];
            }
            for (const auto ed :
                boost::make_iterator_range(out_edges(u, _graph))) {
                expr -= m_flowVars[get(boost::edge_index_t, ed, _graph)];
            }
            retval.emplace_back(IloAdd(m_model, IloRange(expr == 0)));
        }
        return retval;
    }())
    , m_delayConstraints(IloAdd(m_model, IloRange(m_model.getEnv())))

{}

void CompactConstrainedShortestPathModel::setNodePair(
    std::size_t _source, std::size_t _target) {
    m_flowConservationConstraints[m_currentTarget].setBounds(0.0, 0.0);
    m_flowConservationConstraints[m_currentSource].setBounds(0.0, 0.0);
    m_currentSource = _source;
    m_currentTarget = _target;
    m_flowConservationConstraints[m_currentTarget].setBounds(1.0, 1.0);
    m_flowConservationConstraints[m_currentSource].setBounds(1.0, 1.0);
}

template <typename DelayPropertyMap>
void CompactConstrainedShortestPathModel::setDelays(
    const DelayPropertyMap& _delayMap) {
    IloExpr arcSumExpr;
    for (const auto ed : boost::make_iterator_range(edges(_graph))) {
        arcSumExpr += get(_delayPropertyMap, ed, _graph)
                      * m_flowVars[get(boost::edge_index, ed, _graph)];
    }
    m_delayConstraint.setExpr(arcSumExpr);
}

} // namespace CppRO
#endif
